#ifndef UTILS_PAGE_HTML_H
#define UTILS_PAGE_HTML_H

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

#endif //UTILS_PAGE_HTML_H