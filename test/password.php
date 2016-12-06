<?php

$afconfig('password', [
	'length'	=> 8,		//Minimum length
]);


assert(afuser::validatePassword('test') === 'Password must be at least 8 characters long');
assert(afuser::validatePassword('test1324') === 'Password must be at least 8 characters long');



echo 'GOOD!!';
