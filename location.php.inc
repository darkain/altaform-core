<?php

function clean_location_name($location) {
	global $db;
	
	$location = strtolower($location);
	$location = ucwords($location);
	$location = str_replace(',', ', ', $location);
	$location = str_replace("\r", ' ', $location);
	$location = str_replace("\n", ' ', $location);
	$location = str_replace("\t", ' ', $location);
	$location = str_replace('. ', ' ', $location);

	$location = preg_replace('/\\busa\\b/i', '', $location);
	$location = preg_replace('/\\bunited states\\b/i', '', $location);
	$location = preg_replace('/\\bunited states of america\\b/i', '', $location);
	$location = preg_replace('/\s+/', ' ', $location);

	$location = trim($location);

	while (substr($location, -1) === ',') {
		$location = substr($location, 0, strlen($location)-1);
	}

	$states = $db->rows('pudl_state');

	foreach ($states as $state) {
		if (preg_match("/, $state[state_name]\\b/i", $location)) {
			$location = preg_replace("/, $state[state_name]\\b/i", ", $state[state_code]", $location, 1);
			break;
		} else if (preg_match("/, $state[state_code]\\b/i", $location)) {
			$location = preg_replace("/, $state[state_code]\\b/i", ", $state[state_code]", $location, 1);
			break;
		} else if (preg_match("/.\\b$state[state_name]\\b/i", $location)) {
			$location = preg_replace("/.\\b$state[state_name]\\b/i", ", $state[state_code]", $location, 1);
			break;
		} else if (preg_match("/.\\b$state[state_code]\\b/i", $location)) {
			$location = preg_replace("/.\\b$state[state_code]\\b/i", ", $state[state_code]", $location, 1);
			break;
		}
	}

	$location = str_replace(' ,', ',', $location);
	while (stripos($location, ',,') !== false) {
		$location = str_replace(',,', ',', $location);
	}

	return trim($location);
}
