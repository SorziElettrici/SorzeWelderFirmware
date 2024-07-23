#ifndef WEB_HANDLER_H
#define WEB_HANDLER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <Update.h>

#include <pump.h>


//
// Webpages functions
//
void serveHomePage(AsyncWebServerRequest *request, bool pumpState, int pumpSpeed, int pressureSensorValue);
void serveUtilsPage(AsyncWebServerRequest *request);
void serveWifiPage(AsyncWebServerRequest *request);
void serveFirmwareUpdatePage(AsyncWebServerRequest *request);


//
// PUMP Web functions
//
void handlePostPumpSpeed(AsyncWebServerRequest *request, int &pumpSpeed, bool &pumpState, Preferences &preferences);
void handlePostTogglePump(AsyncWebServerRequest *request, int &pumpSpeed, bool &pumpState, Preferences &preferences);


//
// Command handling functions
//
void handleWifiCredentialsSave(AsyncWebServerRequest *request, Preferences &preferences);
void handleWifiCredentialsClear(AsyncWebServerRequest *request, Preferences &preferences);
void handleGetWifiAp(AsyncWebServerRequest *request, Preferences &preferences);
void handleGetPressureValue(AsyncWebServerRequest *request, int &pressureValue);
void handlePostOtaUpdate(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final);



//
// Webpages html
//

constexpr char HTML_HOMEPAGE[] = R"(
    <div class="pure-g">
        <div class="pure-u-1 pure-u-md-1-1">PUMP State: %s</div>
        <div class="pure-u-1 pure-u-md-1-1">
            <button class="pure-button pure-button-primary" onclick="location.href='/toggle'">Toggle LED</button>
        </div>
        <br><br>
        <div class="pure-u-1 pure-u-md-1-1">
            <form class="pure-form pure-form-stacked" action="/set-speed" method="post">
                <div class="pure-g">
                    <div class="pure-u-1-4 pure-u-md-1-1">
                        <label for="pumpspeed">Pump Speed:</label>
                        <input class="pure-u-23-24" type="number" name="pumpspeed" id="pumpspeed" value="%d" min="0" max="255">
                    </div>
                    <div class="pure-u-3-4 pure-u-md-1-1">
                        <label>&nbsp;</label>
                        <button type="submit" class="pure-button pure-button-primary">Set Speed</button>
                    </div>
                </div>
            </form>
        </div>
        <br><br>
        <div class="pure-u-1 pure-u-md-1-1" id="pressure-value">Pressure Sensor Value: </div>
    </div>
    <script>
        setInterval(function() {
            fetch('/get-pressure-value')
            .then(response => response.text())
            .then(text => document.getElementById('pressure-value').innerHTML = "Pressure Sensor Value: " + text)
            .catch(err => console.error('An error occurred: ' + err));
        }, 5000);
    </script>
)";


constexpr char HTML_WIFIPAGE[] = R"(
    <div class="pure-g">
        <div class="pure-u-1 pure-u-md-1-1">
            <form id='wifi-form' class="pure-form pure-form-stacked">
                <div class="pure-g">
                    <div class="pure-u-2-5 pure-u-md-1-1">
                        <label for='ssid'>SSID:</label>
                        <input type='text' name='ssid' id='ssid' style='width: 97%;' placeholder='Current SSID will appear here'>
                    </div>
                    <div class="pure-u-2-5 pure-u-md-1-1">
                        <label for 'password'>Password:</label>
                        <input type='password' name='password' id='password' style='width: 97%;' placeholder='Current password is hidden'>
                    </div>
                    <div class="pure-u-1-5 pure-u-md-1-1">
                        <label>&nbsp;</label>
                        <button type="button" class="pure-button pure-button-primary" onclick='saveWifi()'>Save</button>
                    </div>
                </div>
            </form>
        </div>
        <div class="pure-u-1 pure-u-md-1-1" style='height=10px;'>
            &nbsp;
        </div>
        <div class="pure-u-1 pure-u-md-1-1">
            <button style="margin" class="pure-button pure-button-primary" onclick='clearWifi()'>Clear WiFi Credentials</button>
        </div>
    </div>
    <script>
        window.onload = function() {
            fetch('/current-ap')
            .then(response => response.text())
            .then(ap => {
                var ssidField = document.getElementById('ssid');
                ssidField.placeholder = ap;
            });
        }
        function saveWifi() {
            var form = document.getElementById('wifi-form');
            var data = new FormData(form);
            fetch('/save', {
                method: 'POST',
                body: data
            })
            .then(response => response.text())
            .then(text => showToast(text))
            .catch(err => showToast('An error occurred: ' + err));
        }
        function clearWifi() {
            fetch('/clear-wifi', {method: 'POST'})
            .then(response => response.text())
            .then(text => showToast(text))
            .catch(err => showToast('An error occurred: ' + err));
        }
    </script>
)";

constexpr char HTML_OTAPAGE[] = R"(
    <div class="pure-g">
        <div class="pure-u-1 pure-u-md-1-1">
            <form class="pure-form pure-form-stacked" action='/uploadfimrware' method='POST' enctype='multipart/form-data'>
                <label for="stacked-firmware">Firmware .bin file</label>
                <input id="stacked-firmware" type='file' name='firmware' accept=".bin" required>
                <br><br>
                <button type="submit" class="pure-button pure-button-primary">Upload Firmware</button>
            </form>
        </div>
    </div>
)";

constexpr char HTML_UTILSPAGE[] = R"(
    <div class="pure-g">
        <div class="pure-u-1 pure-u-md-1-1">
            <form class="pure-form pure-form-stacked" id="restartForm">
                <div class="pure-g">
                    <div class="pure-u-1-5 pure-u-md-1-1">
                        <button class="pure-button pure-button-primary" type="button" onclick='restartDevice()'>Restart Device</button>
                    </div>
                </div>
            </form>
        </div>
    </div>
    <script>
        function restartDevice() {
            fetch('/restart', {method: 'POST'})
            .then(response => response.text())
            .then(text => {
                showToast(text);
                setTimeout(checkDeviceStatus, 10000);
            })
            .catch(err => showToast('An error occurred: ' + err));
        }
        
        function checkDeviceStatus() {
            fetch('/')
            .then(response => {
                if (response.ok) {
                    location.reload();
                } else {
                    showToast('Device is not back online yet.');
                }
            })
            .catch(err => showToast('An error occurred: ' + err));
        }
    </script>
)";

constexpr char HTML_MENU[] = R"(
    <div class="pure-menu">
        <a class="pure-menu-heading" href="https://github.com/HopInFog">Let There Be Water<br>HopInFog</a>

        <ul class="pure-menu-list">
            <li class="pure-menu-item"><a href="/" class="pure-menu-link">Home</a></li>
            <li class="pure-menu-item"><a href="/wifi" class="pure-menu-link">WiFi Setup</a></li>
            <li class="pure-menu-item"><a href="/utils" class="pure-menu-link">Utilities</a></li>
            <li class="pure-menu-item"><a href="/firmwareupdate" class="pure-menu-link">Firmware Update</a></li>
        </ul>
    </div>
)";

constexpr char HTML_STRUCTURE[] = R"(
    <!DOCTYPE html>
    <html lang="en">
        <head>
            <meta charset="utf-8">
            <title>%t</title>
            <style>html{line-height:1.15;-webkit-text-size-adjust:100%}body{margin:0}main{display:block}h1{font-size:2em;margin:.67em 0}hr{box-sizing:content-box;height:0;overflow:visible}pre{font-family:monospace,monospace;font-size:1em}a{background-color:transparent}abbr[title]{border-bottom:none;text-decoration:underline;-webkit-text-decoration:underline dotted;text-decoration:underline dotted}b,strong{font-weight:bolder}code,kbd,samp{font-family:monospace,monospace;font-size:1em}small{font-size:80%}sub,sup{font-size:75%;line-height:0;position:relative;vertical-align:baseline}sub{bottom:-.25em}sup{top:-.5em}img{border-style:none}button,input,optgroup,select,textarea{font-family:inherit;font-size:100%;line-height:1.15;margin:0}button,input{overflow:visible}button,select{text-transform:none}[type=button],[type=reset],[type=submit],button{-webkit-appearance:button}[type=button]::-moz-focus-inner,[type=reset]::-moz-focus-inner,[type=submit]::-moz-focus-inner,button::-moz-focus-inner{border-style:none;padding:0}[type=button]:-moz-focusring,[type=reset]:-moz-focusring,[type=submit]:-moz-focusring,button:-moz-focusring{outline:1px dotted ButtonText}fieldset{padding:.35em .75em .625em}legend{box-sizing:border-box;color:inherit;display:table;max-width:100%;padding:0;white-space:normal}progress{vertical-align:baseline}textarea{overflow:auto}[type=checkbox],[type=radio]{box-sizing:border-box;padding:0}[type=number]::-webkit-inner-spin-button,[type=number]::-webkit-outer-spin-button{height:auto}[type=search]{-webkit-appearance:textfield;outline-offset:-2px}[type=search]::-webkit-search-decoration{-webkit-appearance:none}::-webkit-file-upload-button{-webkit-appearance:button;font:inherit}details{display:block}summary{display:list-item}template{display:none}[hidden]{display:none}html{font-family:sans-serif}.hidden,[hidden]{display:none!important}.pure-img{max-width:100%;height:auto;display:block}.pure-g{display:flex;flex-flow:row wrap;align-content:flex-start}.pure-u{display:inline-block;vertical-align:top}.pure-u-1,.pure-u-1-1,.pure-u-1-12,.pure-u-1-2,.pure-u-1-24,.pure-u-1-3,.pure-u-1-4,.pure-u-1-5,.pure-u-1-6,.pure-u-1-8,.pure-u-10-24,.pure-u-11-12,.pure-u-11-24,.pure-u-12-24,.pure-u-13-24,.pure-u-14-24,.pure-u-15-24,.pure-u-16-24,.pure-u-17-24,.pure-u-18-24,.pure-u-19-24,.pure-u-2-24,.pure-u-2-3,.pure-u-2-5,.pure-u-20-24,.pure-u-21-24,.pure-u-22-24,.pure-u-23-24,.pure-u-24-24,.pure-u-3-24,.pure-u-3-4,.pure-u-3-5,.pure-u-3-8,.pure-u-4-24,.pure-u-4-5,.pure-u-5-12,.pure-u-5-24,.pure-u-5-5,.pure-u-5-6,.pure-u-5-8,.pure-u-6-24,.pure-u-7-12,.pure-u-7-24,.pure-u-7-8,.pure-u-8-24,.pure-u-9-24{display:inline-block;letter-spacing:normal;word-spacing:normal;vertical-align:top;text-rendering:auto}.pure-u-1-24{width:4.1667%}.pure-u-1-12,.pure-u-2-24{width:8.3333%}.pure-u-1-8,.pure-u-3-24{width:12.5%}.pure-u-1-6,.pure-u-4-24{width:16.6667%}.pure-u-1-5{width:20%}.pure-u-5-24{width:20.8333%}.pure-u-1-4,.pure-u-6-24{width:25%}.pure-u-7-24{width:29.1667%}.pure-u-1-3,.pure-u-8-24{width:33.3333%}.pure-u-3-8,.pure-u-9-24{width:37.5%}.pure-u-2-5{width:40%}.pure-u-10-24,.pure-u-5-12{width:41.6667%}.pure-u-11-24{width:45.8333%}.pure-u-1-2,.pure-u-12-24{width:50%}.pure-u-13-24{width:54.1667%}.pure-u-14-24,.pure-u-7-12{width:58.3333%}.pure-u-3-5{width:60%}.pure-u-15-24,.pure-u-5-8{width:62.5%}.pure-u-16-24,.pure-u-2-3{width:66.6667%}.pure-u-17-24{width:70.8333%}.pure-u-18-24,.pure-u-3-4{width:75%}.pure-u-19-24{width:79.1667%}.pure-u-4-5{width:80%}.pure-u-20-24,.pure-u-5-6{width:83.3333%}.pure-u-21-24,.pure-u-7-8{width:87.5%}.pure-u-11-12,.pure-u-22-24{width:91.6667%}.pure-u-23-24{width:95.8333%}.pure-u-1,.pure-u-1-1,.pure-u-24-24,.pure-u-5-5{width:100%}.pure-button{display:inline-block;line-height:normal;white-space:nowrap;vertical-align:middle;text-align:center;cursor:pointer;-webkit-user-drag:none;-webkit-user-select:none;user-select:none;box-sizing:border-box}.pure-button::-moz-focus-inner{padding:0;border:0}.pure-button-group{letter-spacing:-.31em;text-rendering:optimizespeed}.opera-only :-o-prefocus,.pure-button-group{word-spacing:-0.43em}.pure-button-group .pure-button{letter-spacing:normal;word-spacing:normal;vertical-align:top;text-rendering:auto}.pure-button{font-family:inherit;font-size:100%;padding:.5em 1em;color:rgba(0,0,0,.8);border:none transparent;background-color:#e6e6e6;text-decoration:none;border-radius:2px}.pure-button-hover,.pure-button:focus,.pure-button:hover{background-image:linear-gradient(transparent,rgba(0,0,0,.05) 40%,rgba(0,0,0,.1))}.pure-button:focus{outline:0}.pure-button-active,.pure-button:active{box-shadow:0 0 0 1px rgba(0,0,0,.15) inset,0 0 6px rgba(0,0,0,.2) inset;border-color:#000}.pure-button-disabled,.pure-button-disabled:active,.pure-button-disabled:focus,.pure-button-disabled:hover,.pure-button[disabled]{border:none;background-image:none;opacity:.4;cursor:not-allowed;box-shadow:none;pointer-events:none}.pure-button-hidden{display:none}.pure-button-primary,.pure-button-selected,a.pure-button-primary,a.pure-button-selected{background-color:#0078e7;color:#fff}.pure-button-group .pure-button{margin:0;border-radius:0;border-right:1px solid rgba(0,0,0,.2)}.pure-button-group .pure-button:first-child{border-top-left-radius:2px;border-bottom-left-radius:2px}.pure-button-group .pure-button:last-child{border-top-right-radius:2px;border-bottom-right-radius:2px;border-right:none}.pure-form input[type=color],.pure-form input[type=date],.pure-form input[type=datetime-local],.pure-form input[type=datetime],.pure-form input[type=email],.pure-form input[type=month],.pure-form input[type=number],.pure-form input[type=password],.pure-form input[type=search],.pure-form input[type=tel],.pure-form input[type=text],.pure-form input[type=time],.pure-form input[type=url],.pure-form input[type=week],.pure-form select,.pure-form textarea{padding:.5em .6em;display:inline-block;border:1px solid #ccc;box-shadow:inset 0 1px 3px #ddd;border-radius:4px;vertical-align:middle;box-sizing:border-box}.pure-form input:not([type]){padding:.5em .6em;display:inline-block;border:1px solid #ccc;box-shadow:inset 0 1px 3px #ddd;border-radius:4px;box-sizing:border-box}.pure-form input[type=color]{padding:.2em .5em}.pure-form input[type=color]:focus,.pure-form input[type=date]:focus,.pure-form input[type=datetime-local]:focus,.pure-form input[type=datetime]:focus,.pure-form input[type=email]:focus,.pure-form input[type=month]:focus,.pure-form input[type=number]:focus,.pure-form input[type=password]:focus,.pure-form input[type=search]:focus,.pure-form input[type=tel]:focus,.pure-form input[type=text]:focus,.pure-form input[type=time]:focus,.pure-form input[type=url]:focus,.pure-form input[type=week]:focus,.pure-form select:focus,.pure-form textarea:focus{outline:0;border-color:#129fea}.pure-form input:not([type]):focus{outline:0;border-color:#129fea}.pure-form input[type=checkbox]:focus,.pure-form input[type=file]:focus,.pure-form input[type=radio]:focus{outline:thin solid #129FEA;outline:1px auto #129FEA}.pure-form .pure-checkbox,.pure-form .pure-radio{margin:.5em 0;display:block}.pure-form input[type=color][disabled],.pure-form input[type=date][disabled],.pure-form input[type=datetime-local][disabled],.pure-form input[type=datetime][disabled],.pure-form input[type=email][disabled],.pure-form input[type=month][disabled],.pure-form input[type=number][disabled],.pure-form input[type=password][disabled],.pure-form input[type=search][disabled],.pure-form input[type=tel][disabled],.pure-form input[type=text][disabled],.pure-form input[type=time][disabled],.pure-form input[type=url][disabled],.pure-form input[type=week][disabled],.pure-form select[disabled],.pure-form textarea[disabled]{cursor:not-allowed;background-color:#eaeded;color:#cad2d3}.pure-form input:not([type])[disabled]{cursor:not-allowed;background-color:#eaeded;color:#cad2d3}.pure-form input[readonly],.pure-form select[readonly],.pure-form textarea[readonly]{background-color:#eee;color:#777;border-color:#ccc}.pure-form input:focus:invalid,.pure-form select:focus:invalid,.pure-form textarea:focus:invalid{color:#b94a48;border-color:#e9322d}.pure-form input[type=checkbox]:focus:invalid:focus,.pure-form input[type=file]:focus:invalid:focus,.pure-form input[type=radio]:focus:invalid:focus{outline-color:#e9322d}.pure-form select{height:2.25em;border:1px solid #ccc;background-color:#fff}.pure-form select[multiple]{height:auto}.pure-form label{margin:.5em 0 .2em}.pure-form fieldset{margin:0;padding:.35em 0 .75em;border:0}.pure-form legend{display:block;width:100%;padding:.3em 0;margin-bottom:.3em;color:#333;border-bottom:1px solid #e5e5e5}.pure-form-stacked input[type=color],.pure-form-stacked input[type=date],.pure-form-stacked input[type=datetime-local],.pure-form-stacked input[type=datetime],.pure-form-stacked input[type=email],.pure-form-stacked input[type=file],.pure-form-stacked input[type=month],.pure-form-stacked input[type=number],.pure-form-stacked input[type=password],.pure-form-stacked input[type=search],.pure-form-stacked input[type=tel],.pure-form-stacked input[type=text],.pure-form-stacked input[type=time],.pure-form-stacked input[type=url],.pure-form-stacked input[type=week],.pure-form-stacked label,.pure-form-stacked select,.pure-form-stacked textarea{display:block;margin:.25em 0}.pure-form-stacked input:not([type]){display:block;margin:.25em 0}.pure-form-aligned input,.pure-form-aligned select,.pure-form-aligned textarea,.pure-form-message-inline{display:inline-block;vertical-align:middle}.pure-form-aligned textarea{vertical-align:top}.pure-form-aligned .pure-control-group{margin-bottom:.5em}.pure-form-aligned .pure-control-group label{text-align:right;display:inline-block;vertical-align:middle;width:10em;margin:0 1em 0 0}.pure-form-aligned .pure-controls{margin:1.5em 0 0 11em}.pure-form .pure-input-rounded,.pure-form input.pure-input-rounded{border-radius:2em;padding:.5em 1em}.pure-form .pure-group fieldset{margin-bottom:10px}.pure-form .pure-group input,.pure-form .pure-group textarea{display:block;padding:10px;margin:0 0 -1px;border-radius:0;position:relative;top:-1px}.pure-form .pure-group input:focus,.pure-form .pure-group textarea:focus{z-index:3}.pure-form .pure-group input:first-child,.pure-form .pure-group textarea:first-child{top:1px;border-radius:4px 4px 0 0;margin:0}.pure-form .pure-group input:first-child:last-child,.pure-form .pure-group textarea:first-child:last-child{top:1px;border-radius:4px;margin:0}.pure-form .pure-group input:last-child,.pure-form .pure-group textarea:last-child{top:-2px;border-radius:0 0 4px 4px;margin:0}.pure-form .pure-group button{margin:.35em 0}.pure-form .pure-input-1{width:100%}.pure-form .pure-input-3-4{width:75%}.pure-form .pure-input-2-3{width:66%}.pure-form .pure-input-1-2{width:50%}.pure-form .pure-input-1-3{width:33%}.pure-form .pure-input-1-4{width:25%}.pure-form-message-inline{display:inline-block;padding-left:.3em;color:#666;vertical-align:middle;font-size:.875em}.pure-form-message{display:block;color:#666;font-size:.875em}@media only screen and (max-width :480px){.pure-form button[type=submit]{margin:.7em 0 0}.pure-form input:not([type]),.pure-form input[type=color],.pure-form input[type=date],.pure-form input[type=datetime-local],.pure-form input[type=datetime],.pure-form input[type=email],.pure-form input[type=month],.pure-form input[type=number],.pure-form input[type=password],.pure-form input[type=search],.pure-form input[type=tel],.pure-form input[type=text],.pure-form input[type=time],.pure-form input[type=url],.pure-form input[type=week],.pure-form label{margin-bottom:.3em;display:block}.pure-group input:not([type]),.pure-group input[type=color],.pure-group input[type=date],.pure-group input[type=datetime-local],.pure-group input[type=datetime],.pure-group input[type=email],.pure-group input[type=month],.pure-group input[type=number],.pure-group input[type=password],.pure-group input[type=search],.pure-group input[type=tel],.pure-group input[type=text],.pure-group input[type=time],.pure-group input[type=url],.pure-group input[type=week]{margin-bottom:0}.pure-form-aligned .pure-control-group label{margin-bottom:.3em;text-align:left;display:block;width:100%}.pure-form-aligned .pure-controls{margin:1.5em 0 0 0}.pure-form-message,.pure-form-message-inline{display:block;font-size:.75em;padding:.2em 0 .8em}}.pure-menu{box-sizing:border-box}.pure-menu-fixed{position:fixed;left:0;top:0;z-index:3}.pure-menu-item,.pure-menu-list{position:relative}.pure-menu-list{list-style:none;margin:0;padding:0}.pure-menu-item{padding:0;margin:0;height:100%}.pure-menu-heading,.pure-menu-link{display:block;text-decoration:none;white-space:nowrap}.pure-menu-horizontal{width:100%;white-space:nowrap}.pure-menu-horizontal .pure-menu-list{display:inline-block}.pure-menu-horizontal .pure-menu-heading,.pure-menu-horizontal .pure-menu-item,.pure-menu-horizontal .pure-menu-separator{display:inline-block;vertical-align:middle}.pure-menu-item .pure-menu-item{display:block}.pure-menu-children{display:none;position:absolute;left:100%;top:0;margin:0;padding:0;z-index:3}.pure-menu-horizontal .pure-menu-children{left:0;top:auto;width:inherit}.pure-menu-active>.pure-menu-children,.pure-menu-allow-hover:hover>.pure-menu-children{display:block;position:absolute}.pure-menu-has-children>.pure-menu-link:after{padding-left:.5em;content:"\25B8";font-size:small}.pure-menu-horizontal .pure-menu-has-children>.pure-menu-link:after{content:"\25BE"}.pure-menu-scrollable{overflow-y:scroll;overflow-x:hidden}.pure-menu-scrollable .pure-menu-list{display:block}.pure-menu-horizontal.pure-menu-scrollable .pure-menu-list{display:inline-block}.pure-menu-horizontal.pure-menu-scrollable{white-space:nowrap;overflow-y:hidden;overflow-x:auto;padding:.5em 0}.pure-menu-horizontal .pure-menu-children .pure-menu-separator,.pure-menu-separator{background-color:#ccc;height:1px;margin:.3em 0}.pure-menu-horizontal .pure-menu-separator{width:1px;height:1.3em;margin:0 .3em}.pure-menu-horizontal .pure-menu-children .pure-menu-separator{display:block;width:auto}.pure-menu-heading{text-transform:uppercase;color:#565d64}.pure-menu-link{color:#777}.pure-menu-children{background-color:#fff}.pure-menu-heading,.pure-menu-link{padding:.5em 1em}.pure-menu-disabled{opacity:.5}.pure-menu-disabled .pure-menu-link:hover{background-color:transparent;cursor:default}.pure-menu-active>.pure-menu-link,.pure-menu-link:focus,.pure-menu-link:hover{background-color:#eee}.pure-menu-selected>.pure-menu-link,.pure-menu-selected>.pure-menu-link:visited{color:#000}.pure-table{border-collapse:collapse;border-spacing:0;empty-cells:show;border:1px solid #cbcbcb}.pure-table caption{color:#000;font:italic 85%/1 arial,sans-serif;padding:1em 0;text-align:center}.pure-table td,.pure-table th{border-left:1px solid #cbcbcb;border-width:0 0 0 1px;font-size:inherit;margin:0;overflow:visible;padding:.5em 1em}.pure-table thead{background-color:#e0e0e0;color:#000;text-align:left;vertical-align:bottom}.pure-table td{background-color:transparent}.pure-table-odd td{background-color:#f2f2f2}.pure-table-striped tr:nth-child(2n-1) td{background-color:#f2f2f2}.pure-table-bordered td{border-bottom:1px solid #cbcbcb}.pure-table-bordered tbody>tr:last-child>td{border-bottom-width:0}.pure-table-horizontal td,.pure-table-horizontal th{border-width:0 0 1px 0;border-bottom:1px solid #cbcbcb}.pure-table-horizontal tbody>tr:last-child>td{border-bottom-width:0}</style>
            <style>
                body {
                    color: #777;
                }

                .pure-img-responsive {
                    max-width: 100%;
                    height: auto;
                }

                /*
                Add transition to containers so they can push in and out.
                */
                #layout,
                #menu,
                .menu-link {
                    -webkit-transition: all 0.2s ease-out;
                    -moz-transition: all 0.2s ease-out;
                    -ms-transition: all 0.2s ease-out;
                    -o-transition: all 0.2s ease-out;
                    transition: all 0.2s ease-out;
                }

                /*
                This is the parent `<div>` that contains the menu and the content area.
                */
                #layout {
                    position: relative;
                    left: 0;
                    padding-left: 0;
                }
                    #layout.active #menu {
                        left: 230px;
                        width: 230px;
                    }

                    #layout.active .menu-link {
                        left: 230px;
                    }
                /*
                The content `<div>` is where all your content goes.
                */
                .content {
                    margin: 0 auto;
                    padding: 0.5em 2em 0 2em;
                    max-width: 800px;
                    margin-bottom: 50px;
                    line-height: 1.6em;
                }

                .header {
                    margin: 0;
                    color: #333;
                    text-align: center;
                    padding: 1.40em 2em 0;
                    border-bottom: 1px solid #eee;
                }
                    .header h1 {
                        margin: 0 0 0.5em 0;
                        font-size: 3em;
                        font-weight: 300;
                    }
                    .header h2 {
                        font-weight: 300;
                        color: #ccc;
                        padding: 0;
                        margin-top: 0;
                    }

                .content-subhead {
                    margin: 50px 0 20px 0;
                    font-weight: 300;
                    color: #888;
                }



                /*
                The `#menu` `<div>` is the parent `<div>` that contains the `.pure-menu` that
                appears on the left side of the page.
                */

                #menu {
                    margin-left: -230px; /* "#menu" width */
                    width: 230px;
                    position: fixed;
                    top: 0;
                    left: 0;
                    bottom: 0;
                    z-index: 1000; /* so the menu or its navicon stays above all content */
                    background: #191818;
                    overflow-y: auto;
                }
                    /*
                    All anchors inside the menu should be styled like this.
                    */
                    #menu a {
                        color: #999;
                        border: none;
                        padding: 0.6em 0 0.6em 0.6em;
                    }

                    /*
                    Remove all background/borders, since we are applying them to #menu.
                    */
                    #menu .pure-menu,
                    #menu .pure-menu ul {
                        border: none;
                        background: transparent;
                    }

                    /*
                    Add that light border to separate items into groups.
                    */
                    #menu .pure-menu ul,
                    #menu .pure-menu .menu-item-divided {
                        border-top: 1px solid #333;
                    }
                        /*
                        Change color of the anchor links on hover/focus.
                        */
                        #menu .pure-menu li a:hover,
                        #menu .pure-menu li a:focus {
                            background: #333;
                        }

                    /*
                    This styles the selected menu item `<li>`.
                    */
                    #menu .pure-menu-selected {
                        background: #191818;
                    }
                    #menu .pure-menu-selected::before, #menu .pure-menu-heading::before {
                        content: "";
                        position: absolute;
                        left: 0;
                        top: 0;
                        bottom: 0;
                        width: 5px;
                        background: #1f8dd6;
                    }

                    #menu .pure-menu-selected, #menu .pure-menu-heading {
                        position: relative;
                    }
                    #menu .pure-menu-heading {
                        background: #1f8dd6;
                    }
                        /*
                        This styles a link within a selected menu item `<li>`.
                        */
                        #menu .pure-menu-selected a {
                            color: #fff;
                        }

                    /*
                    This styles the menu heading.
                    */
                    #menu .pure-menu-heading {
                        font-size: 110%;
                        color: #fff;
                        margin: 0;
                    }

                /* -- Dynamic Button For Responsive Menu -------------------------------------*/

                /*
                The button to open/close the Menu is custom-made and not part of Pure. Here's
                how it works:
                */

                /*
                `.menu-link` represents the responsive menu toggle that shows/hides on
                small screens.
                */
                .menu-link {
                    position: fixed;
                    display: block; /* show this only on small screens */
                    top: 0;
                    left: 0; /* "#menu width" */
                    background: #000;
                    background: rgba(0,0,0,0.7);
                    font-size: 10px; /* change this value to increase/decrease button size */
                    z-index: 10;
                    width: 2em;
                    height: auto;
                    padding: 2.1em 1.6em;
                }

                    .menu-link:hover,
                    .menu-link:focus {
                        background: #000;
                    }

                    .menu-link span {
                        position: relative;
                        display: block;
                    }

                    .menu-link span,
                    .menu-link span:before,
                    .menu-link span:after {
                        background-color: #fff;
                        pointer-events: none;
                        width: 100%;
                        height: 0.2em;
                    }

                        .menu-link span:before,
                        .menu-link span:after {
                            position: absolute;
                            margin-top: -0.6em;
                            content: " ";
                        }

                        .menu-link span:after {
                            margin-top: 0.6em;
                        }


                /*
                Toast
                */
                .toast {
                    visibility: hidden;
                    max-width: 50%;
                    margin: auto;
                    background-color: #333;
                    color: #fff;
                    text-align: center;
                    border-radius: 2px;
                    padding: 16px;
                    position: fixed;
                    z-index: 1;
                    left: 25%;
                    bottom: 30px;
                    font-size: 17px;
                    transition: visibility 0.5s, opacity 0.5s;
                }
                .toast.show {
                    visibility: visible;
                    opacity: 1;
                }

                /* -- Responsive Styles (Media Queries) ------------------------------------- */

                /*
                Hides the menu at `48em`, but modify this based on your app's needs.
                */
                @media (min-width: 48em) {

                    .header,
                    .content {
                        padding-left: 2em;
                        padding-right: 2em;
                    }

                    #layout {
                        padding-left: 150px; /* left col width "#menu" */
                        left: 0;
                    }
                    #menu {
                        left: 230px;
                    }

                    .menu-link {
                        position: fixed;
                        left: 150px;
                        display: none;
                    }

                    #layout.active .menu-link {
                        left: 230px;
                    }
                }

                @media (max-width: 48em) {
                    /* Only apply this when the window is small. Otherwise, the following
                    case results in extra padding on the left:
                        * Make the window small.
                        * Tap the menu to trigger the active state.
                        * Make the window large again.
                    */
                    #layout.active {
                        position: relative;
                        left: 150px;
                    }
                }
            </style>
            <meta name="viewport" content="width=device-width, initial-scale=1">
        </head>
        <body>
            <div id="layout">
                <!-- Menu toggle -->
                <a href="#menu" id="menuLink" class="menu-link">
                    <!-- Hamburger icon -->
                    <span></span>
                </a>

                <div id="menu">%m</div>
            
                <div id="main">
                    <div class="header">
                        <h1>%t</h1>
                    </div>
                    <div class="content">
                        %b
                    </div>
                    <div id="toast" class="toast">
                        <p id="toast-content"></p>
                    </div>
                </div>
            </div>
            <script type="text/javascript">!function(e,n){function t(){return{layout:n.getElementById("layout"),menu:n.getElementById("menu"),menuLink:n.getElementById("menuLink")}}function i(e,n){for(var t=e.className.split(/\s+/),i=t.length,u=0;u<i;u++)if(t[u]===n){t.splice(u,1);break}i===t.length&&t.push(n),e.className=t.join(" ")}function u(){var e="active",n=t();i(n.layout,e),i(n.menu,e),i(n.menuLink,e)}n.addEventListener("click",(function(e){var n=t();e.target.id===n.menuLink.id?(u(),e.preventDefault()):-1!==n.menu.className.indexOf("active")&&u()}))}(0,this.document);</script>
            <script>
                window.addEventListener('DOMContentLoaded', () => {
                    const menuItems = document.querySelectorAll('.pure-menu-item a');
                    const currentUrl = "/" + window.location.href.split('#')[0].split('/').pop();

                    menuItems.forEach(menuItem => {
                        menuUrl = "/" + menuItem.href.split('#')[0].split('/').pop();
                        menuUrl === currentUrl ? menuItem.parentNode.classList.add('pure-menu-selected') : menuItem.parentNode.classList.remove('pure-menu-selected');
                    });
                });
            </script>
            <script>
                function showToast(message) {
                    var toast = document.getElementById("toast");
                    var content = document.getElementById("toast-content");
                    content.innerHTML = message;
                    toast.className = "toast show";
                    setTimeout(function(){ toast.className = toast.className.replace("show", ""); }, 3000);
                }
            </script>
        </body>
    </html>
)";

#endif //WEB_HANDLER_H