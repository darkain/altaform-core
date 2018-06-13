<?php



function afUnit($result, $expected=true) {
	if ($result === $expected) return;
	$trace = debug_backtrace()[0];
	echo "\n\n";
	echo "ERROR: FAILED!!\n\n";
	echo "PHP:\t" . PHP_VERSION . "\n";
	echo "FILE:\t$trace[file]\n";
	echo "LINE:\t$trace[line]\n\n";
	echo "EXPECTED:\n";
	var_dump($expected);
	echo "\n\n";
	echo "RESULT:\n";
	var_dump($result);
	exit(1);
}



$list = scandir(__DIR__);


foreach ($list as $item) {
	if (strtolower(substr($item, -8)) !== '.inc.php') continue;
	require_once(__DIR__ . '/' . $item);
}
