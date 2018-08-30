<?php


// DISABLE DOUBLE-LOADING
if (class_exists('altaform', false)) return;


// TBX - THE RENDERING SYSTEM
require_once(af_file_owner('_tbx/tbx.php.inc'));


// CORE FUNCTIONALITY
require_once(af_file_owner(__DIR__.'/core/afIp.inc.php'));
require_once(af_file_owner(__DIR__.'/core/af2fa.inc.php'));
require_once(af_file_owner(__DIR__.'/core/afCli.inc.php'));
require_once(af_file_owner(__DIR__.'/core/afDir.inc.php'));
require_once(af_file_owner(__DIR__.'/core/afGeo.inc.php'));
require_once(af_file_owner(__DIR__.'/core/afGit.inc.php'));
require_once(af_file_owner(__DIR__.'/core/afFile.inc.php'));
require_once(af_file_owner(__DIR__.'/core/afMail.inc.php'));
require_once(af_file_owner(__DIR__.'/core/afTime.inc.php'));
require_once(af_file_owner(__DIR__.'/core/afVoid.inc.php'));
require_once(af_file_owner(__DIR__.'/core/afAudit.inc.php'));
require_once(af_file_owner(__DIR__.'/core/afConfig.inc.php'));
require_once(af_file_owner(__DIR__.'/core/afDevice.inc.php'));
require_once(af_file_owner(__DIR__.'/core/afString.inc.php'));
require_once(af_file_owner(__DIR__.'/core/afUpload.inc.php'));
require_once(af_file_owner(__DIR__.'/core/afYoutube.inc.php'));
require_once(af_file_owner(__DIR__.'/core/afActivity.inc.php'));
require_once(af_file_owner(__DIR__.'/core/afMimetype.inc.php'));
require_once(af_file_owner(__DIR__.'/core/afFunctions.inc.php'));


// LIST OF ALTAFORM TRAITS - USED TO HELP ORGANIZE CODE
require_once(af_file_owner(__DIR__.'/traits/afAuth.inc.php'));
require_once(af_file_owner(__DIR__.'/traits/afNode.inc.php'));
require_once(af_file_owner(__DIR__.'/traits/afRobots.inc.php'));
require_once(af_file_owner(__DIR__.'/traits/afRouter.inc.php'));
require_once(af_file_owner(__DIR__.'/traits/afEncrypt.inc.php'));
require_once(af_file_owner(__DIR__.'/traits/afCallable.inc.php'));
require_once(af_file_owner(__DIR__.'/traits/afTemplate.inc.php'));


// ALTAFORM CORE
require_once(af_file_owner(__DIR__.'/core/afCore.inc.php'));
