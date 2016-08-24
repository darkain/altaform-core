<?php

function parse_etime($s) {
	$m = array();
	preg_match("/^(([\d]+)-)?(([\d]+):)?([\d]+):([\d]+)$/", trim($s), $m);
	return
		$m[2]*86400+	//Days
		$m[4]*3600+		//Hours
		$m[5]*60+		//Minutes
		$m[6];			//Seconds
}

$output = [
	'type'		=> 'NGINX',
	'version'	=> $_SERVER['SERVER_SOFTWARE'],
	'arch'		=> (PHP_INT_SIZE*8) . '-bit',
	'boot'		=> time(),
	'memory'	=> 0,
];


$list = explode("\n", `ps x -O vsz,etime | grep [n]ginx`);
foreach ($list as $item) {
	$item = explode(' ', trim(preg_replace('/(?:\s\s+|\t)/', ' ', $item)));
	if (empty($item)  ||  count($item)<3) continue;

	$boot = time() - parse_etime($item[2]);
	$output['boot'] = min($output['boot'], $boot);

	$output['memory'] += ($item[1] * 1024);
}


echo json_encode($output);
