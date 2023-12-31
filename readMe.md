<!DOCTYPE html>
<html>
<head>
    <title>Sensor Board Project</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 40px;
        }
        h1, h2 {
            color: #333;
        }
        a {
            color: #007bff;
            text-decoration: none;
        }
        a:hover {
            text-decoration: underline;
        }
    </style>
</head>
<body>

    <h1>Sensor Board</h1>

    <h2>About This Project</h2>
    <p>This project includes all the resources needed to create and expand your own Sensor Board. It's perfect for hobbyists, developers, and anyone interested in microcontroller and sensor technology.</p>

    <h2>Installation</h2>
    <h3>Prerequisites</h3>
    <ul>
        <li>An <strong>ESP32-DevKit with WiFi support</strong>. Find details and documentation <a href="https://www.amazon.de/AZDelivery-ESP32-NodeMCU-Module-Parent/dp/B0BF5NRF8H?th=1">here</a>.</li>
        <li>An Arduino board, preferably an <a href="https://www.amazon.de/Mikrocontroller-ATmega2560-ATMEGA16U2-USB-Kabel-Kompatibel/dp/B01MA5BLQI/ref=sr_1_3?__mk_de_DE=ÅMÅŽÕÑ&crid=1OVJ2F3DOYQMR&keywords=arduino+mega+2560&qid=1701462329&sprefix=arduino+mega+2560%2Caps%2C114&sr=8-3">Arduino Mega 2560</a> for ample port availability.</li>
    </ul>

    <h3>Step-by-Step Instructions</h3>
    <ol>
        <li><strong>Hardware Preparation</strong>: Ensure you have set up the ESP32-DevKit and the Arduino board as per instructions.</li>
        <li><strong>Software Setup</strong>:
            <ul>
                <li>Upload all necessary files to your microcontroller.</li>
                <li>Update your credentials in the <code>Credentials</code> example file.</li>
                <li>We recommend using <a href="https://code.visualstudio.com">VS-Code</a> with the <a href="https://platformio.org">PlatformIO</a> plugin for an efficient development environment.</li>
            </ul>
        </li>
        <li><strong>Web Interface Configuration</strong>:
            <ul>
                <li>Update your HTML page as needed.</li>
                <li>Upload the <a href="https://github.com/littlefs-project/littlefs">Filesystem</a> for file management.</li>
            </ul>
        </li>
    </ol>

    <h2>Support & Community</h2>
    <p>Have questions or want to engage in discussions? Join our community or contact us directly for support.</p>

    <h2>Contributing</h2>
    <p>Contributions are warmly welcomed! If you have ideas on how this project can be improved, feel free to create a pull request or open an issue.</p>

</body>
</html>
