# Blink Example

This example demonstrates how to blink a LED the [led_strip](https://components.espressif.com/component/espressif/led_strip) component for the addressable LED, i.e. [WS2812](https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf).

The `led_strip` is installed via [component manager](main/idf_component.yml).

The following is the web link to search for the components, this is important because in the Component Manager inside Eclipse IDE, there is no option to search.  
[Component Manager Website](https://components.espressif.com)

I added a submodule named `esp-idf-component`, and my plan was to use one of it's library, but unfortunately for the time being I have to drop the plan because this submodule also contains a `led_strip` drivers and it is creating problem with the drivers installed with `IDF Component Manager`.