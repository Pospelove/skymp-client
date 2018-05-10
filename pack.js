// require modules
var fs = require('fs-extra');
var path = require('path');

const REQUIREMENTS = path.join(__dirname, '/requirements');

const MYGUI_FONT = path.join(__dirname, '/requirements/MyGUI/futuralightc1.otf');
const MYGUI_FONT__ = MYGUI_FONT + '__';

const FILES_REPO = path.join(__dirname, '..', 'skymp-client-files');

fs.renameSync(MYGUI_FONT, MYGUI_FONT__);

fs.copySync(REQUIREMENTS, FILES_REPO);
fs.copySync(__dirname + '/Release/SkympClient.dll', path.join(FILES_REPO, 'Data/skse/plugins/SkympClient.dll'));

fs.renameSync(MYGUI_FONT__, MYGUI_FONT);
