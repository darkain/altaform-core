<?php

$output	= [
	'type'					=> defined('HHVM_VERSION') ? 'HHVM' : 'PHP',
	'version'				=> defined('HHVM_VERSION') ? HHVM_VERSION : PHP_VERSION,
	'arch'					=> (PHP_INT_SIZE*8) . '-bit',
	'boot'					=> time(),
	'memory'				=> 0,
	'pid'					=> [],
	'max_post_size'			=> ini_get('post_max_size'),
	'max_file_size'			=> ini_get('upload_max_filesize'),
];



$pids = [];

if (defined('HHVM_VERSION')) {
	$pids[] = posix_getpid();

} else {
	$list = explode("\n", `ps -eo%mem,pid,command | grep [p]hp-fpm`);
	foreach ($list as $item) {
		$item				= trim($item);
		if (empty($item)) continue;

		$pid				= 0;
		@sscanf($item, '%f %d', $null, $pid);
		if (empty($pid)) continue;

		$pids[]				= $pid;
	}
}



foreach ($pids as $pid) {
	$pidx = ['boot'=>0, 'memory'=>0];

	$stats					= @stat("/proc/$pid/cmdline");
	if (!empty($stats['ctime'])) {
		$output['boot']		= min($output['boot'], $stats['ctime']);
		$pidx['boot']		= $stats['ctime'];
	}

	$data					= explode(' ', @file_get_contents("/proc/$pid/stat"));
	if (!empty($data[22])) {
		$output['memory']	+= $data[22];
		$pidx['memory']		= $data[22];
	}

	$output['pid'][]		= $pidx;
}



$af->json($output);
