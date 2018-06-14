<?php

$base = $afurl->base;
$afurl->base = '/foo';



afUnit(
	$afurl('test'),
	'test'
);



afUnit(
	$afurl(['test']),
	'test'
);




afUnit(
	$afurl('test', true),
	'/foo/test'
);




afUnit(
	$afurl(['test'], true),
	'/foo/test'
);



afUnit(
	$afurl(['test', 'bar']),
	'test/bar'
);



afUnit(
	$afurl(['test', 'bar'], true),
	'/foo/test/bar'
);




$afurl->base = $base;
