<?php


$db->create('pudl_altaform', [
	'af_key'	=> 'varchar(255) NOT NULL',
	'af_value'	=> 'varchar(21588) NOT NULL',
], false, 'ENGINE=InnoDB DEFAULT CHARSET=utf8mb4');


$db(
	'ALTER TABLE ' . $db->identifiers('pudl_altaform', true) .
	'	ADD PRIMARY KEY (' . $db->identifier('af_key') . '),' .
	'	ADD KEY ' . $db->identifier('af_value') .
		' (' . $db->identifier('af_value') . '(255))'
);
