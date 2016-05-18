<?php

require_once('geolocation.php.inc');



class afaudit {


	static function insert($object, $type, $value_old, $value_new) {
		global $af, $db, $user;

		$db->insert('pudl_audit', [
			'user_id'			=> $user['user_id'],
			'user_ip'			=> afgeo::ipaddress(),
			'object_id'			=> (int) $object,
			'object_type_id'	=> $af->type($type),
			'audit_old'			=> $value_old,
			'audit_new'			=> $value_new,
			'audit_timestamp'	=> $db->time(),
		]);
	}



	static function action($object, $type, $action) {
		global $af, $db, $user;

		$db->insert('pudl_audit', [
			'user_id'			=> $user['user_id'],
			'user_ip'			=> afgeo::ipaddress(),
			'object_id'			=> (int) $object,
			'object_type_id'	=> $af->type($type),
			'audit_action'		=> $action,
			'audit_timestamp'	=> $db->time(),
		]);
	}


}
