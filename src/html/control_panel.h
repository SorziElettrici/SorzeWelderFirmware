#ifndef CONTROL_PANEL_HTML_H
#define CONTROL_PANEL_HTML_H

constexpr char HTML_CONTROL_PANEL[] = R"(
    <div class="pure-g">

        <div class="pure-u-1 pure-u-md-1-1">
            <div id="zapp-state" class="pure-u-1 pure-u-md-1-1">ZAPP State: %zs%</div>
            <div class="spacer pure-u-1 pure-u-md-1-1">&nbsp</div>
            <button style="margin" class="pure-button pure-button-primary" onclick='zappCommand()'>ZAPP!</button>
        </div>

        <div class="pure-u-1 pure-u-md-1-1">
            <div class="spacer pure-u-1 pure-u-md-1-1">&nbsp</div>
            <div class="spacer pure-u-1 pure-u-md-1-1">&nbsp</div>
            <form id='mains-frequency-form' class="pure-form pure-form-stacked">
                <div class="pure-g">
                    <div class="pure-u-1-3 pure-u-md-1-1">
                        <label for="mains-frequency">Mains Frequency:</label>
                        <input class="pure-u-23-24" type="number" name="mains-frequency" id="mains-frequency" value="%fv%" step="10" min="50" max="60">
                    </div>
                    <div class="pure-u-4-4 pure-u-md-1-1">
                        <label>&nbsp;</label>
                        <button type="button" class="pure-button pure-button-primary" onclick='saveMainsFrequency()'>Set Mains Frequency</button>
                    </div>
                </div>
            </form>
            <div class="pure-u-1 pure-u-md-1-1">HalfCycle duration (ms): %hcd%</div>
        </div>
        
        <div class="pure-u-1 pure-u-md-1-1">
            <div class="spacer pure-u-1 pure-u-md-1-1">&nbsp</div>
            <div class="spacer pure-u-1 pure-u-md-1-1">&nbsp</div>
            <form id='preheat-halfcycles-form' class="pure-form pure-form-stacked">
                <div class="pure-g">
                    <div class="pure-u-1-3 pure-u-md-1-1">
                        <label for="preheat-halfcycles">PreHeat HalfCycles:</label>
                        <input class="pure-u-23-24" type="number" name="preheat-halfcycles" id="preheat-halfcycles" value="%phhc%" min="0" max="10">
                    </div>
                    <div class="pure-u-4-4 pure-u-md-1-1">
                        <label>&nbsp;</label>
                        <button type="button" class="pure-button pure-button-primary" onclick='savePreheatHalfCycles()'>Set PreHeat HalfCycles</button>
                    </div>
                </div>
            </form>
            <div class="pure-u-1 pure-u-md-1-1">PreHeat duration (ms): %phd%</div>
        </div>
        
        <div class="pure-u-1 pure-u-md-1-1">
            <div class="spacer pure-u-1 pure-u-md-1-1">&nbsp</div>
            <div class="spacer pure-u-1 pure-u-md-1-1">&nbsp</div>
            <form id='preheat-zapp-interval-halfcycles-form' class="pure-form pure-form-stacked">
                <div class="pure-g">
                    <div class="pure-u-1-3 pure-u-md-1-1">
                        <label for="preheat-zapp-interval-halfcycles">PreHeat Zapp Interval HalfCycles:</label>
                        <input class="pure-u-23-24" type="number" name="preheat-zapp-interval-halfcycles" id="preheat-zapp-interval-halfcycles" value="%phzhc%" min="0" max="10">
                    </div>
                    <div class="pure-u-4-4 pure-u-md-1-1">
                        <label>&nbsp;</label>
                        <button type="button" class="pure-button pure-button-primary" onclick='savePreheatZappIntervalHalfCycles()'>Set PreHeat Zapp Interval HalfCycles</button>
                    </div>
                </div>
            </form>
            <div class="pure-u-1 pure-u-md-1-1">PreHeat Zapp Interval duration (ms): %phzid%</div>
        </div>
        
        <div class="pure-u-1 pure-u-md-1-1">
            <div class="spacer pure-u-1 pure-u-md-1-1">&nbsp</div>
            <div class="spacer pure-u-1 pure-u-md-1-1">&nbsp</div>
            <form id='zapp-halfcycles-form' class="pure-form pure-form-stacked">
                <div class="pure-g">
                    <div class="pure-u-1-3 pure-u-md-1-1">
                        <label for="zapp-halfcycles">Zapp HalfCycles:</label>
                        <input class="pure-u-23-24" type="number" name="zapp-halfcycles" id="zapp-halfcycles" value="%zhc%" min="0" max="20">
                    </div>
                    <div class="pure-u-4-4 pure-u-md-1-1">
                        <label>&nbsp;</label>
                        <button type="button" class="pure-button pure-button-primary" onclick='saveZappHalfCycles()'>Set Zapp HalfCycles</button>
                    </div>
                </div>
            </form>
            <div class="pure-u-1 pure-u-md-1-1">Zapp duration (ms): %zd%</div>
        </div>
        
        <div class="pure-u-1 pure-u-md-1-1">
            <div class="spacer pure-u-1 pure-u-md-1-1">&nbsp</div>
            <div class="spacer pure-u-1 pure-u-md-1-1">&nbsp</div>
            <button style="margin" class="pure-button pure-button-primary" onclick='resetZappSettings()'>Reset Zapp Settings</button>
        </div>
    </div>
    <script>
        function saveMainsFrequency() {
            var form = document.getElementById('mains-frequency-form');
            var data = new FormData(form);
            fetch('/set-mains-frequency', {
                method: 'POST',
                body: data
            })
            .then(response => response.text())
            .then(text => showToast(text))
            .catch(err => showToast('An error occurred: ' + err));
        }
        function savePreheatHalfCycles() {
            var form = document.getElementById('preheat-halfcycles-form');
            var data = new FormData(form);
            fetch('/set-preheat-halfcycles', {
                method: 'POST',
                body: data
            })
            .then(response => response.text())
            .then(text => showToast(text))
            .catch(err => showToast('An error occurred: ' + err));
        }
        function savePreheatZappIntervalHalfCycles() {
            var form = document.getElementById('preheat-zapp-interval-halfcycles-form');
            var data = new FormData(form);
            fetch('/set-preheat-zapp-interval-halfcycles', {
                method: 'POST',
                body: data
            })
            .then(response => response.text())
            .then(text => showToast(text))
            .catch(err => showToast('An error occurred: ' + err));
        }
        function saveZappHalfCycles() {
            var form = document.getElementById('zapp-halfcycles-form');
            var data = new FormData(form);
            fetch('/set-zapp-halfcycles', {
                method: 'POST',
                body: data
            })
            .then(response => response.text())
            .then(text => showToast(text))
            .catch(err => showToast('An error occurred: ' + err));
        }
        function zappCommand() {
            fetch('/zapp-command', {method: 'POST'})
            .then(response => response.text())
            .then(text => showToast(text))
            .catch(err => showToast('An error occurred: ' + err));
        }
        function resetZappSettings() {
            fetch('/reset-zapp-settings', {method: 'POST'})
            .then(response => response.text())
            .then(text => showToast(text))
            .catch(err => showToast('An error occurred: ' + err));
        }
    </script>
)";

#endif //CONTROL_PANEL_HTML_H