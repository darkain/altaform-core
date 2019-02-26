<?php


// DISABLE DOUBLE-LOADING
if (class_exists('altaform', false)) return;


// TBX - THE RENDERING SYSTEM
require_once(is_owner('_tbx/tbx.inc.php'));


// NEW ALTAFORM 3.0 INDEPENDENT MODULES
require_once(is_owner(__DIR__.'/router/router.php'));
require_once(is_owner(__DIR__.'/modules/ip.php'));
require_once(is_owner(__DIR__.'/modules/cli.php'));
require_once(is_owner(__DIR__.'/modules/git.php'));
require_once(is_owner(__DIR__.'/modules/otp.php'));
require_once(is_owner(__DIR__.'/modules/file.php'));
require_once(is_owner(__DIR__.'/modules/mime.php'));
require_once(is_owner(__DIR__.'/modules/path.php'));
require_once(is_owner(__DIR__.'/modules/abyss.php'));


// ALTAFORM 2.0 CORE FUNCTIONALITY
require_once(is_owner(__DIR__.'/core/afGeo.inc.php'));
require_once(is_owner(__DIR__.'/core/afMail.inc.php'));
require_once(is_owner(__DIR__.'/core/afTime.inc.php'));
require_once(is_owner(__DIR__.'/core/afAudit.inc.php'));
require_once(is_owner(__DIR__.'/core/afConfig.inc.php'));
require_once(is_owner(__DIR__.'/core/afDevice.inc.php'));
require_once(is_owner(__DIR__.'/core/afStatus.inc.php'));
require_once(is_owner(__DIR__.'/core/afString.inc.php'));
require_once(is_owner(__DIR__.'/core/afSystem.inc.php'));
require_once(is_owner(__DIR__.'/core/afUpload.inc.php'));
require_once(is_owner(__DIR__.'/core/afYoutube.inc.php'));
require_once(is_owner(__DIR__.'/core/afActivity.inc.php'));
require_once(is_owner(__DIR__.'/core/afFunctions.inc.php'));


// LIST OF ALTAFORM TRAITS - USED TO HELP ORGANIZE CODE
require_once(is_owner(__DIR__.'/traits/afAuth.inc.php'));
require_once(is_owner(__DIR__.'/traits/afNode.inc.php'));
require_once(is_owner(__DIR__.'/traits/afRobots.inc.php'));
require_once(is_owner(__DIR__.'/traits/afEncrypt.inc.php'));
require_once(is_owner(__DIR__.'/traits/afCallable.inc.php'));
require_once(is_owner(__DIR__.'/traits/afTemplate.inc.php'));


// ALTAFORM CORE
require_once(is_owner(__DIR__.'/core/afCore.inc.php'));
