<?php

class afactivity {

	public static function add($id, $type, $verb=NULL) {
		global $af, $db, $user;

		if (empty($user['user_id'])) return false;

		return $db->insert('pudl_activity', [
			'activity_timestamp'	=> (int) $db->time(),
			'user_id'				=> (int) $user['user_id'],
			'object_id'				=> (int) $id,
			'object_type_id'		=> (int) $af->type($type),
			'activity_verb'			=> $verb,
		], [
			'activity_count'		=> pudl::_increment(1),
			'activity_timestamp'	=> $db->time(),
		]);
	}


	public static function delete($id, $type) {
		global $af, $db, $user;

		if (empty($user['user_id'])) return false;

		return $db->delete('pudl_activity', [
			'user_id'			=> (int) $user['user_id'],
			'object_id'			=> (int) $id,
			'object_type_id'	=> (int) $af->type($type),
		]);
	}

}
