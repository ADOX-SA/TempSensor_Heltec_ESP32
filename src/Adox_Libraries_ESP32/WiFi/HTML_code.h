#include <Arduino.h>



static const char HTML_HEADER[] = R"(
  <!DOCTYPE html>
<html>
<head>
      <meta charset=utf-8>
      <title>ADOX</title>
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
<style>
ul {
  list-style-type: none;
  margin: 0;
  padding: 0;
  overflow: hidden;
  background-color: #333;
}

li {
  float: left;
}

li a {
  display: block;
  color: white;
  text-align: center;
  padding: 14px 16px;
  text-decoration: none;
}

li a:hover:not(.active) {
  background-color: #C42525;
}

</style>
</head>
<body>

<ul>
  <li><a href='/'>Inicio</a></li>
  <li><a href='/wifi'>WiFi</a></li>
  <li><a href='/download'>Download</a></li>
  <li><a href='/date'>Fecha</a></li>
  <li><a href='/sensed'>Config. envio</a></li>
  <li><a href='/ultimodato'>Ult. envio</a></li>
  <li><a href='/config_mqtt'>MQTT</a></li>
  <li><a href='/realtimedata'>Real time data</a>

</ul>
</body>
<body>
  )";

static const char HTML_END[] = R"(
    </body>
    </html>
  )";