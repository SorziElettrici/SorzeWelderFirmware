#ifndef FAN_PANEL_HTML_H
#define FAN_PANEL_HTML_H

constexpr char HTML_FAN_PANEL[] = R"(
    <div class="pure-g">
        <div class="pure-u-1 pure-u-md-1-1">
            <div id="zapp-state" class="pure-u-1 pure-u-md-1-1">Electronics temperature (°C): %et%</div>
            <div id="zapp-state" class="pure-u-1 pure-u-md-1-1">Transformer temperature (°C): %tt%</div>
        </div>

        <div class="pure-u-1 pure-u-md-1-1">
            <div class="spacer pure-u-1 pure-u-md-1-1">&nbsp</div>
            <div class="spacer pure-u-1 pure-u-md-1-1">&nbsp</div>
            <div class="pure-u-1 pure-u-md-1-1 pure-button-group" role="group" aria-label="fan-mode-button-group">
                <button style="margin" class="pure-button pure-button-primary" onclick='setFanOff()'>Off</button>
                <button style="margin" class="pure-button pure-button-primary" onclick='setFanAuto()'>Auto</button>
                <button style="margin" class="pure-button pure-button-primary" onclick='setFanOn()'>On</button>
            </div>
            <div class="spacer pure-u-1 pure-u-md-1-1">&nbsp</div>
            <div id="fan-mode" class="pure-u-1 pure-u-md-1-1">Current Fan Mode: %cfm%</div>
        </div>

        <div class="pure-u-1 pure-u-md-1-1">
            <div class="spacer pure-u-1 pure-u-md-1-1">&nbsp</div>
            <div class="spacer pure-u-1 pure-u-md-1-1">&nbsp</div>
            <form id='fan-speeds-form' class="pure-form pure-form-stacked">
                <div class="pure-g">
                    <div class="pure-u-1-3 pure-u-md-1-1">
                        <label for="min-fan-speed">Min Fan Speed (%):</label>
                        <input class="pure-u-23-24" type="number" name="min-fan-speed" id="min-fan-speed" value="%mifs%" step="1" min="0" max="100">
                    </div>
                </div>
                <div class="pure-g">
                    <div class="pure-u-1-3 pure-u-md-1-1">
                        <label for="max-fan-speed">Max Fan Speed (%):</label>
                        <input class="pure-u-23-24" type="number" name="max-fan-speed" id="max-fan-speed" value="%mafs%" step="1" min="0" max="100">
                    </div>
                    <div class="pure-u-4-4 pure-u-md-1-1">
                        <label>&nbsp;</label>
                        <button type="button" class="pure-button pure-button-primary" onclick='saveFanSpeeds()'>Set Fan Speeds</button>
                    </div>
                </div>
            </form>
            <div class="spacer pure-u-1 pure-u-md-1-1">&nbsp</div>
            <div id="fan-speed" class="pure-u-1 pure-u-md-1-1">Current Fan Speed: %cfs%</div>
        </div>

        <div class="pure-u-1 pure-u-md-1-1">
            <div class="spacer pure-u-1 pure-u-md-1-1">&nbsp</div>
            <div class="spacer pure-u-1 pure-u-md-1-1">&nbsp</div>
            <button style="margin" class="pure-button pure-button-primary" onclick='resetFanSettings()'>Reset Fan Settings</button>
        </div>
    </div>
    <script>
        function saveFanSpeeds() {
            var form = document.getElementById('fan-speeds-form');
            var data = new FormData(form);
            fetch('/set-fan-speeds', {
                method: 'POST',
                body: data
            })
            .then(response => response.text())
            .then(text => showToast(text))
            .catch(err => showToast('An error occurred: ' + err));
        }

        function setFanOff() {
            fetch('/set-fan-off', {method: 'POST'})
            .then(response => response.text())
            .then(text => showToast(text))
            .catch(err => showToast('An error occurred: ' + err));
        }

        function setFanAuto() {
            fetch('/set-fan-auto', {method: 'POST'})
            .then(response => response.text())
            .then(text => showToast(text))
            .catch(err => showToast('An error occurred: ' + err));
        }

        function setFanOn() {
            fetch('/set-fan-on', {method: 'POST'})
            .then(response => response.text())
            .then(text => showToast(text))
            .catch(err => showToast('An error occurred: ' + err));
        }

        function resetFanSettings() {
            fetch('/reset-fan-settings', {method: 'POST'})
            .then(response => response.text())
            .then(text => showToast(text))
            .catch(err => showToast('An error occurred: ' + err));
        }
    </script>
)";

#endif //FAN_PANEL_HTML_H