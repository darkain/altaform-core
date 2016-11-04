<?php

if (!$af->debug()) $user->requireStaff();


$title = 'Server Status';

$af->script($afurl->static.'/js/jquery.tablesorter.min.js');
$af->style($afurl->static.'/css/theme.blue.css');


if (empty($servers)  ||  !tbx_array($servers)) {
	error500('NO SERVERS SPECIFIED, OR INVALID SERVER FORMAT');
}



////////////////////////////////////////////////////////////
// WEB SERVERS
////////////////////////////////////////////////////////////
$ctx = stream_context_create(['http'=>[
	'timeout'			=> 5,
	'follow_location'	=> false,
]]);

foreach ($servers as &$server) {
	$nstime = microtime(true);

	$json = pudl::jsonDecode(@file_get_contents($server, 0, $ctx));
	if (!empty($json)) {
		$server = [
			'path'		=> $server,
			'delay'		=> sprintf('%0.4f', microtime(true) - $nstime),
			'uptime'	=> str_replace('ago', '', aftime::since($json['boot'], AF_YEAR*7)),
			'memory'	=> afstring::fromBytes( empty($json['memory']) ? 0 : $json['memory'] ),
		] + $json;
	} else {
		$server = [
			'path'		=> $server,
			'uptime'	=> 'offline',
		];
	}
} unset($server);




////////////////////////////////////////////////////////////
// REDIS SERVER
////////////////////////////////////////////////////////////
if ($db->redis() instanceof Redis) {
	$nstime	= microtime(true);
	$redis	= $db->redis()->info();
	$servers[] = [
		'path'			=> 'Redis-01',
		'version'		=> $redis['redis_version'],
		'boot'			=> $af->time() - $redis['uptime_in_seconds'],
		'uptime'		=> aftime::since($af->time() - $redis['uptime_in_seconds'], AF_YEAR*7),
		'memory'		=> afstring::fromBytes($redis['used_memory']),
		'delay'			=> sprintf('%0.4f', microtime(true) - $nstime),
	];
} else {
	$servers[]			= [
		'path'			=> 'Redis-01',
		'uptime'		=> 'OFFLINE!',
	];
}




////////////////////////////////////////////////////////////
// DATABASE SERVERS
////////////////////////////////////////////////////////////
$databases = [];

$list = $db->status('wsrep_incoming_addresses');
if (!empty($list['wsrep_incoming_addresses'])) {
	$list = str_replace(':3306', '', $list['wsrep_incoming_addresses']);
	$list = explode(',', $list);
	foreach ($list as $item) {
		$item = trim($item);
		if (empty($item)) continue;
		if (!in_array($item, $databases)) $databases[] = $item;
	}
}

sort($databases);


pudlMySqli::dieOnError(false);
foreach ($databases as $item) {
	$name				= $item;
	$nstime				= microtime(true);
	$connect			= new pudlMySqli([$db, 'server'=>$name, 'backup'=>'']);

	if (!empty($replacers[0])  &&  !empty($replacers[1])) {
		$name = str_replace($replacers[0], $replacers[1], $name);
	}

	if ($connect->connectErrno()) {
		$servers[] = [
			'path'		=> $name,
			'version'	=> $connect->connectErrno() . ' : ' . $connect->connectError(),
		];

	} else {
		$uptime			= $connect->globals('Uptime');
		$version		= $connect->variables('version');
		$memory			= $connect->status('Innodb_buffer_pool_bytes_data');

		$servers[]		= [
			'path'		=> $name,
			'version'	=> !empty($version['version']) ? $version['version'] : NULL,
			'boot'		=> !empty($uptime['Uptime']) ? $af->time() - $uptime['Uptime'] : NULL,
			'uptime'	=> !empty($uptime['Uptime']) ? aftime::since($af->time() - $uptime['Uptime'], AF_YEAR*7) : NULL,
			'memory'	=> !empty($memory['Innodb_buffer_pool_bytes_data']) ? afstring::fromBytes($memory['Innodb_buffer_pool_bytes_data']) : NULL,
			'delay'		=> sprintf('%0.4f', microtime(true) - $nstime),
		];

		if ($db instanceof pudlGalera) {
			$db->onlineServer($item);
		}
	}
}



$offline = $db->offlineServers();
foreach ($offline as $item) {
	$servers[] = [
		'path'			=> $item,
		'version'		=> 'OFFLINE',
	];
}




////////////////////////////////////////////////////////////
// OUTPUT ALL THE THINGS !!!
////////////////////////////////////////////////////////////
$af->header();
	$af->renderBlock('_index.tpl', 'server', $servers);
$af->footer();
