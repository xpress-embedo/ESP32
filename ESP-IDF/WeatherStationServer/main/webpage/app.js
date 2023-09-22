/**
 * Add gobals here
 */
var seconds = null;
var otaTimerVar = null;
var wifiConnectInterval = null;

/**
 * Initialize functions here.
 */
$(document).ready(function()
{
  getSSID();
  getUpdateStatus();
  startSensorInterval();
  startLocalTimeInterval();
  // earlier I commented out this function, but this is also important
  // for the scenarios when the user has refreshed the web page
  getConnectInfo();

  // Connect Button Callback can be done in HTML code also
  $("#connect_wifi").on("click", function() {
    checkCredentials();
  });

  // Disconnect Button Callback can be done in HTML code also
  $("#disconnect_wifi").on("click", function() {
    disconnectWiFi();
  });
});

/**
 * Gets file name and size for display on the web page.
 */
function getFileInfo() 
{
  var x = document.getElementById("selected_file");
  var file = x.files[0];

  document.getElementById("file_info").innerHTML = "<h4>File: " + file.name + "<br>" + "Size: " + file.size + " bytes</h4>";
}

/**
 * Handles the firmware update.
 */
function updateFirmware() 
{
  // Form Data
  var formData = new FormData();
  var fileSelect = document.getElementById("selected_file");
  
  if (fileSelect.files && fileSelect.files.length == 1) 
  {
    var file = fileSelect.files[0];
    formData.set("file", file, file.name);
    document.getElementById("ota_update_status").innerHTML = "Uploading " + file.name + ", Firmware Update in Progress...";

    // Http Request
    var request = new XMLHttpRequest();

    request.upload.addEventListener("progress", updateProgress);
    request.open('POST', "/OTAupdate");
    request.responseType = "blob";
    request.send(formData);
  }
  else 
  {
    window.alert('Select A File First')
  }
}

/**
 * Progress on transfers from the server to the client (downloads).
 */
function updateProgress(oEvent) 
{
  if (oEvent.lengthComputable) 
  {
    getUpdateStatus();
  } 
  else 
  {
    window.alert('total size is unknown')
  }
}

/**
 * Posts the firmware udpate status.
 */
function getUpdateStatus() 
{
  var xhr = new XMLHttpRequest();
  var requestURL = "/OTAstatus";
  xhr.open('POST', requestURL, false);
  xhr.send('ota_update_status');

  if (xhr.readyState == 4 && xhr.status == 200)
  {
    var response = JSON.parse(xhr.responseText);

    document.getElementById("latest_firmware").innerHTML = response.compile_date + " - " + response.compile_time

    // If flashing was complete it will return a 1, else -1
    // A return of 0 is just for information on the Latest Firmware request
    if (response.ota_update_status == 1)
    {
      // Set the countdown timer time
      seconds = 10;
      // Start the countdown timer
      otaRebootTimer();
      } 
    else if (response.ota_update_status == -1)
    {
      document.getElementById("ota_update_status").innerHTML = "!!! Upload Error !!!";
    }
  }
}

/**
 * Displays the reboot countdown.
 */
function otaRebootTimer() 
{
  document.getElementById("ota_update_status").innerHTML = "OTA Firmware Update Complete. This page will close shortly, Rebooting in: " + seconds;

  if (--seconds == 0) 
  {
    clearTimeout(otaTimerVar);
    window.location.reload();
  } 
  else 
  {
    otaTimerVar = setTimeout(otaRebootTimer, 1000);
  }
}

// Get the Sensor Temperature and Humidity Values for Display on the web page
function getSensorValues()
{
  $.getJSON('/Sensor', function(data) {
    $("#temperature_value").text(data["temp"]);
    $("#humidity_value").text(data["humidity"]);
  });
}

// Sets the Interval for getting the updated Sensor Values
function startSensorInterval()
{
  // Call this function every 5 seconds
  setInterval(getSensorValues, 5000);
}

// Clear the Connection Status Interval
function stopWiFiConnectStatusInterval()
{
  if( wifiConnectInterval != null )
  {
    clearInterval(wifiConnectInterval);
    wifiConnectInterval = null;
  }
}

// Gets the WiFi Connection Status
function getWiFiConnectStatus()
{
  var xhr = new XMLHttpRequest();
  var requestURL = "/wifiConnectStatus";
  xhr.open('POST', requestURL, false);
  xhr.send('wifi_connect_status');
  
  if( (xhr.readyState == 4) && (xhr.status == 200) )
  {
    var response = JSON.parse(xhr.responseText);
    document.getElementById("wifi_connect_status").innerHTML = "Connecting.....";

    if( response.wifi_connect_status == 2 )
    {
      document.getElementById("wifi_connect_status").innerHTML = "<h4 class='rd'>Failed to Connect. Please check AP credentials and compatibility</h4>";
      stopWiFiConnectStatusInterval();
    }
    else if( response.wifi_connect_status == 3 )
    {
      document.getElementById("wifi_connect_status").innerHTML = "<h4 class='gr'>Connection Success!</h4>";
      stopWiFiConnectStatusInterval();
      getConnectInfo();
    }
  }
}

// Starts the interval for checking the connection status
function startWiFiConnectStatusInterval()
{
  // call the function every 3 seconds
  wifiConnectInterval = setInterval( getWiFiConnectStatus, 3000);
}

// Connect WiFi function called using the SSID and Password Entered into the text field
function connectWiFi()
{
  // Get the SSID and Password
  selectedSSID = $("#connect_ssid").val();
  pswd = $("#connect_pswd").val();
  
  $.ajax({
    url: '/wifiConnect',
    dataType: 'json',
    method: 'POST',
    cache: false,
    headers: {'my-connect-ssid': selectedSSID, 'my-connect-pswd': pswd},
    data: { 'timestamp': Date.now() }
  });

  startWiFiConnectStatusInterval();
}

// Check the Entered Connection when "Connect" button is pressed
function checkCredentials()
{
  // console prints are used for debugging
  // console.log("Connect Button is Pressed, now checking the Credentials");
  errorList = "";
  credsOk = true;

  selectedSSID = $("#connect_ssid").val();
  pswd = $("#connect_pswd").val();
  // console.log("SSID:" + selectedSSID);   // used for debugging

  // SSID shouldn't be blank
  if( selectedSSID == "" )
  {
    errorList += "<h4 class='rd'>SSID Can't be Empty!</h4>";
    credsOk = false;
  }
  if( pswd == "" )
  {
    errorList += "<h4 class='rd'>Password Can't be Empty!</h4>";
    credsOk = false;
  }

  // if there is an error, then display it using errorList
  if( credsOk == false )
  {
    $("#wifi_connect_credentials_errors").html(errorList);
  }
  else
  {
    $("#wifi_connect_credentials_errors").html("");
    connectWiFi();
  }
}

// Show the WiFi Password if the box is checked
function showPassword()
{
  var x = document.getElementById("connect_pswd");
  if( x.type === "password")
  {
    x.type = "text";
  }
  else
  {
    x.type = "password";
  }
}

// Get the connection information for displaying on the web page
function getConnectInfo()
{
  $.getJSON('/wifiConnectInfo', function(data) {
    // Console prints were used for debugging
    // console.log( data );
    // console.log( data["ap"]);
    // console.log( data["ip"]);
    // console.log( data["netmask"]);
    // console.log( data["gw"]);
    $("#connected_ap_label").html("Connected to: ");
    $("#connected_ap").text(data["ap"]);

    $("#ip_address_label").html("IP Address: ");
    $("#wifi_connect_ip").text(data["ip"]);

    $("#netmask_label").html("NetMask: ");
    $("#wifi_connect_netmask").text(data["netmask"]);

    $("#gateway_label").html("Gateway: ");
    $("#wifi_connect_gateway").text(data["gw"]);

    document.getElementById('disconnect_wifi').style.display = 'block';
  });
}

// Disconnects the WiFi once the disconnect button is pressed and reloads webpage
function disconnectWiFi()
{
  $.ajax({
    url: '/wifiDisconnect',
    dataType: 'json',
    method: 'DELETE',
    cache: false,
    data: { 'timestamp': Date.now() }
  });

  // Update the webpage
  setTimeout("location.reload(true)", 2000);
}

// Sets the interval for displaying local time
function startLocalTimeInterval()
{
  // call function getLocalTime every 10 seconds
  setInterval(getLocalTime, 10000);
}

// Gets the Local Time
// NOTE: Connect the ESP32 to the internet and the time will be updated
function getLocalTime()
{
  $.getJSON('/localTime', function(data) {
    // console.log(data);   // used for debugging
    $("#local_time").text(data["time"]);
  });
}

// Get the ESP32 Access Point SSID for displaying on the webpage
function getSSID()
{
  $.getJSON('/apSSID', function(data) {
    // console.log(data);   // used for debugging
    $("#ap_ssid").text(data["ssid"]);
  });
}