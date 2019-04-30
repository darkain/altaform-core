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



afUnit(
	$afurl->query('test'),
	'test'
);



afUnit(
	$afurl->query(['test']),
	'0=test'
);



afUnit(
	$afurl->query(['x'=>'test']),
	'x=test'
);



afUnit(
	$afurl->query(['x'=>'one', 'y'=>2, 'z'=>3.5]),
	'x=one&y=2&z=3.5'
);



afUnit(
	$afurl->query(['x'=>'a + b']),
	'x=a%20%2B%20b'
);



afUnit(
	$afurl->query(['x'=>'this is a test']),
	'x=this%20is%20a%20test'
);



afUnit(
	$afurl->query(['x'=>"this\tis\ra\ntest\0"]),
	'x=this%09is%0Da%0Atest%00'
);



afUnit(
	$afurl->query(['x'=>md5('test')]),
	'x=098f6bcd4621d373cade4e832627b4f6'
);



afUnit(
	$afurl->query(['x'=>md5('test', true)]),
	'x=%09%8Fk%CDF%21%D3s%CA%DEN%83%26%27%B4%F6'
);



$afurl->base = $base;
