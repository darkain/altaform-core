<?php


// DISABLE DOUBLE-LOADING
if (class_exists('altaform', false)) return;


// TBX - THE RENDERING SYSTEM
require_once(is_owner('_tbx/tbx.inc.php'));

// INCLUDE EXCEPTIONS BEFORE EVERYTHING ELSE
require_once(is_owner(__DIR__.'/modules/exceptions.php'));

// ALTAFORM 3.0 URL ROUTER MODULES
require_once(is_owner(__DIR__.'/router/router.php'));

// PRE-LOAD MAIN ALTAFORM 3.0 MODULES
\af\module('x');
\af\module('ip');
\af\module('cli');
\af\module('geo');
\af\module('git');
\af\module('otp');
\af\module('dump');
\af\module('file');
\af\module('mail');
\af\module('mime');
\af\module('path');
\af\module('time');
\af\module('abyss');
\af\module('audit');
\af\module('device');
\af\module('import');
\af\module('status');
\af\module('system');
\af\module('sidebar');
\af\module('youtube');
\af\module('activity');


// ALTAFORM 2.0 CORE FUNCTIONALITY
require_once(is_owner(__DIR__.'/core/afConfig.inc.php'));
require_once(is_owner(__DIR__.'/core/afString.inc.php'));


// LIST OF ALTAFORM TRAITS - USED TO HELP ORGANIZE CODE
require_once(is_owner(__DIR__.'/traits/afAuth.inc.php'));
require_once(is_owner(__DIR__.'/traits/afRobots.inc.php'));
require_once(is_owner(__DIR__.'/traits/afEncrypt.inc.php'));
require_once(is_owner(__DIR__.'/traits/afCallable.inc.php'));
require_once(is_owner(__DIR__.'/traits/afTemplate.inc.php'));


// ALTAFORM CORE
require_once(is_owner(__DIR__.'/core/afCore.inc.php'));
