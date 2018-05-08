<?php

if (!$af->debug()) $user->requireStaff();


$af->title = 'Server Status';

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

	//$key	= afUser::password();
	//$hash	= $af->config->auth($key, true);
	$url	= $server;// . '?m=' . urlencode($key) . '&h=' . urlencode($hash);
	$json	= pudl::jsonDecode(@file_get_contents($url, 0, $ctx));

	if (!empty($json)) {
		$server = [
			'path'		=> $server,
			'delay'		=> sprintf('%0.4f', microtime(true) - $nstime),
			'uptime'	=> str_replace('ago', '', afTime::since($json['boot'], AF_YEAR*7)),
			'memory'	=> afString::fromBytes( empty($json['memory']) ? 0 : $json['memory'] ),
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
		'uptime'		=> afTime::since($af->time() - $redis['uptime_in_seconds'], AF_YEAR*7),
		'memory'		=> afString::fromBytes($redis['used_memory']),
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
		$state			= $connect->globals('wsrep_local_state');

		$servers[]		= [
			'path'		=> $name,
			'version'	=> !empty($version) ? (reset($version) . ' : ' . reset($state)) : NULL,
			'boot'		=> !empty($uptime) ? $af->time() - reset($uptime) : NULL,
			'uptime'	=> !empty($uptime) ? afTime::since($af->time() - reset($uptime), AF_YEAR*7) : NULL,
			'memory'	=> !empty($memory) ? afString::fromBytes(reset($memory)) : NULL,
			'delay'		=> sprintf('%0.4f', microtime(true) - $nstime),
		];

		if ($db instanceof pudlGalera) {
			$db->onlineServer($item);
		}
	}
}



$offline = $db->offlineServers();
foreach ($offline as $name => $time) {
	if (($db->time() - $time) > (AF_MINUTE*10)) {
		$db->onlineServer($name);
	} else {
		$servers[] = [
			'path'			=> $name,
			'boot'			=> $time,
			'memory'		=> '0 Bytes',
			'delay'			=> '0.0000',
			'uptime'		=> 'OFFLINE',
			'version'		=> 'OFFLINE',
		];
	}
}




////////////////////////////////////////////////////////////
// OUTPUT ALL THE THINGS !!!
////////////////////////////////////////////////////////////
$af->header();
	$af->renderBlock('_index.tpl', 'server', $servers);
$af->footer();
