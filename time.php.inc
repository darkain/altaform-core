<?php

function timeSince($timestamp) {
	global $db;
	if ($timestamp == 0) return "Never";

	// array of time period chunks
	$chunks = array(
		array(60 * 60 * 24 * 365 , 'year'),
		array(60 * 60 * 24 * 30 , 'month'),
		array(60 * 60 * 24 * 7, 'week'),
		array(60 * 60 * 24 , 'day'),
		array(60 * 60 , 'hour'),
		array(60 , 'minute'),
	);

	$since = $db->time() - $timestamp;

	if ($since == 0) return 'Now';
	if ($since == 1) return '1 second ago';
	if ($since < 60) return "$since seconds ago";

	if ($since > 60*60*24) return date('F jS, Y \a\t g:i A', $timestamp);

	// $j saves performing the count function each time around the loop
	for ($i = 0, $j = count($chunks); $i < $j; $i++) {
		$seconds = $chunks[$i][0];
		$name = $chunks[$i][1];
		// finding the biggest chunk (if the chunk fits, break)
		if (($count = floor($since / $seconds)) > 1) break;
	}

	$print = ($count == 1) ? '1 '.$name : "$count {$name}s";

	for ($x = $i+1;	$x < $j;	$x++) {
		// now getting the second item
		$seconds2 = $chunks[$x][0];
		$name2 = $chunks[$x][1];

		// add second item if it's count greater than 0
		if (($count2 = floor(($since - ($seconds * $count)) / $seconds2)) != 0) {
			$print .= ($count2 == 1) ? ', 1 '.$name2 : ", $count2 {$name2}s";
			break;
		}
	}
	
	return $print . ' ago';
}
