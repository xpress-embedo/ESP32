/**
 * Add gobals here
 */
var wifiConnectInterval = null;

/**
 * Initialize functions here.
 */
$(document).ready(function()
{
  // getSSID();
  // getUpdateStatus();
  
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
			// when connection is successful, we can get the connection info and update the webpage
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














    










    


