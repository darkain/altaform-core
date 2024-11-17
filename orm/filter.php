<?php


////////////////////////////////////////////////////////////
//FILTER DEACTIVATED AND BANNED USERS
////////////////////////////////////////////////////////////
function af_filter_banned($prefixed=true) {
	if (!$prefixed) {
		return ['user_permission' => [4,5]];
	}

	if ($prefixed === true) $prefixed = 'us';

	return [$prefixed.'.user_permission' => [4, 5]];
}



