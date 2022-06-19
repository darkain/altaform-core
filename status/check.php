<?php
/*
//VERIFY SECURITY
if (!$user->isStaff()) {
	if (!$af->config->verify($get->h, $get->m, true)) {
		\af\error(404);
	}
}
*/

$output	= [
	'type'					=> defined('HHVM_VERSION') ? 'HHVM' : 'PHP',
	'version'				=> defined('HHVM_VERSION') ? HHVM_VERSION : PHP_VERSION,
	'arch'					=> trim(`uname -mp`),	// Call OS "uname" command
	'boot'					=> time(),
	'memory'				=> 0,
	'pid'					=> [],
	'max_post_size'			=> ini_get('post_max_size'),
	'max_file_size'			=> ini_get('upload_max_filesize'),
];




$pids[] = posix_getpid();
foreach ($pids as $key => $pid) {
	$uptime	= explode('-', trim(exec('ps -o etime= ' . $pid)));
	$days	= (count($uptime)>1) ? $uptime[0] : 0;
	$clock	= explode(':', end($uptime));

	$output['pid'][] = [
		'boot'		=> ($days*60*60*24) + ($clock[0]*60*60) + ($clock[1]*60) + $clock[2],
		'memory'	=> ((int)trim(exec('ps -o rss= ' . $pid))) * 1024,
	];

	if ($key === 0) {
		$output['boot']		= time() - end($output['pid'])['boot'];
		$output['memory']	= end($output['pid'])['memory'];
	}
}



$af->json($output);
