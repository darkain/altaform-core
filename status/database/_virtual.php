<?php

////////////////////////////////////////////////////////////////////////////////
// ADMIN ACCESS ONLY
////////////////////////////////////////////////////////////////////////////////
$user->requireStaff();




////////////////////////////////////////////////////////////////////////////////
// QUERY SERVER AVAILABILITY
////////////////////////////////////////////////////////////////////////////////
$server				= $router->virtual;
$nstime				= microtime(true);

try {
	$connect		= new pudlMySqli([
		$db,
		'server'	=> $server,
		'backup'	=> '',
		'timeout'	=> 2,
	]);

} catch (pudlConnectionException $e) {
	$decode = @json_decode($e->getMessage());

	return (json_last_error() === JSON_ERROR_NONE)
		? $af->json($decode)
		: $af->json([
			'server'	=> $server,
			'error'		=> $e->getMessage(),
			'code'		=> $e->getCode(),
		]);
}


$uptime			= $connect->globals('Uptime');
$version		= $connect->variables('version');
$memory			= $connect->status('Innodb_buffer_pool_bytes_data');
$readonly		= $connect->readonly() ? ' (READ ONLY)' : false;

$state			= current($connect->globals('wsrep_local_state'))
				. ' : '
				. current($connect->variables('system_versioning_alter_history'));

$servers[]		= [
	'server'	=> $server,
	'version'	=> !empty($version) ? (reset($version) . $readonly . ' : ' . $state) : NULL,
	'boot'		=> !empty($uptime) ? $af->time() - reset($uptime) : NULL,
	'uptime'	=> !empty($uptime) ? afTime::since($af->time() - reset($uptime), AF_YEAR*7) : NULL,
	'memory'	=> !empty($memory) ? afString::fromBytes(reset($memory)) : NULL,
	'delay'		=> sprintf('%0.4f', microtime(true) - $nstime),
];

if ($db instanceof pudlGalera) {
	$db->onlineServer($server);
}


$connect->disconnect();


$af->json($servers);
