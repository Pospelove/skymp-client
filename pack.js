// require modules
var fs = require('fs-extra');
var path = require('path');

const CLIENT_LOGIC_H = path.join(__dirname, 'SkympClient/ClientLogic/ClientLogic.h');
let clh = fs.readFileSync(CLIENT_LOGIC_H, 'utf-8');
let ver = '';
let strings = clh.split(/\r?\n/);
strings.forEach(el => {
    let tokens = el.split(' ');
    if (tokens[0] == '#define' && tokens[1] == 'version' && tokens[2] != undefined) {
        ver = tokens[2].split('"')[1];
    }
});

if (ver.length == 0 || ver.length >= 10) {
    console.log('unable to detect version');
    return;
}

let config = {
  "client_version": ver,
  "server_ip": "127.0.0.1",
  "server_port": "7777",
  "server_password": "123456",
  "translate": "ru"
};
console.log(config);
fs.writeFileSync('config.json', JSON.stringify(config));

const REQUIREMENTS = path.join(__dirname, '/requirements');

const MYGUI_FONT = path.join(__dirname, '/requirements/MyGUI/futuralightc1.otf');
const MYGUI_FONT__ = MYGUI_FONT + '__';

const FILES_REPO = path.join(__dirname, '..', 'skymp-client-files');

fs.renameSync(MYGUI_FONT, MYGUI_FONT__);

fs.copySync(REQUIREMENTS, FILES_REPO);
fs.copySync(__dirname + '/Release/SkympClient.dll', path.join(FILES_REPO, 'Data/skse/plugins/SkympClient.dll'));
fs.copySync(__dirname + '/config.json', path.join(FILES_REPO, 'config.json'));

fs.renameSync(MYGUI_FONT__, MYGUI_FONT);

console.log('\npacked');
