<?php


$__af_test_total__ = 0;

function afUnit($result, $expected=true) {
	global $__af_test_total__;
	$__af_test_total__++;

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



require_once(__DIR__.'/../core/afFile.inc.php');



//PREP THE DIRECTORY
$parent	= dirname(dirname(__DIR__));
$dir	= substr(__DIR__, strlen($parent)-strlen(__DIR__)+1);
$list	= scandir(__DIR__);
shuffle($list);



//RUN ALL UNIT TESTS
foreach ($list as $item) {
	if (strtolower(substr($item, -8)) !== '.inc.php') continue;
	echo afCli::fgWhite("Testing:\t");
	echo afCli::fgCyan($dir.'/');
	echo afCli::fgCyan(1,$item) . "\n";
	require_once(__DIR__ . '/' . $item);
}
