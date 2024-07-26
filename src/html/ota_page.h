#ifndef OTA_PAGE_HTML_H
#define OTA_PAGE_HTML_H

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

#endif //OTA_PAGE_HTML_H