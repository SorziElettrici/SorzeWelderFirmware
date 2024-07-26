#ifndef WIFI_PAGE_HTML_H
#define WIFI_PAGE_HTML_H

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

#endif //WIFI_PAGE_HTML_H