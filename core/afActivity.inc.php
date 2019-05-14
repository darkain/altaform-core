<?php

class afActivity {


	public static function add($id, $type, $verb, $userid=false) {
		global $af, $db;

		$userid = static::uid($userid);
		if (empty($userid)) return false;

		return $db->insert('activity', [
			'activity_timestamp'	=> $db->time(),
			'user_id'				=> $userid,
			'object_id'				=> $id,
			'object_type_id'		=> $af->type($type),
			'activity_verb'			=> $verb,
		], [
			'activity_count'		=> pudl::_increment(1),
			'activity_timestamp'	=> $db->time(),
		]);
	}



	public static function addFile($hash, $type, $verb, $userid=false) {
		global $af, $db;

		$userid = static::uid($userid);
		if (empty($userid)) return false;

		return $db->insert('activity', [
			'activity_timestamp'	=> $db->time(),
			'user_id'				=> $userid,
			'file_hash'				=> $hash,
			'object_type_id'		=> $af->type($type),
			'activity_verb'			=> $verb,
		], [
			'activity_count'		=> pudl::_increment(1),
			'activity_timestamp'	=> $db->time(),
		]);
	}



	public static function delete($id, $type, $userid=false) {
		global $af, $db;

		$userid = static::uid($userid);
		if (empty($userid)) return false;

		return $db->delete('activity', [
			'user_id'			=> $userid,
			'object_id'			=> $id,
			'object_type_id'	=> $af->type($type),
		]);
	}



	public static function deleteFile($hash, $type, $userid=false) {
		global $af, $db;

		$userid = static::uid($userid);
		if (empty($userid)) return false;

		return $db->delete('activity', [
			'user_id'			=> $userid,
			'file_hash'			=> $hash,
			'object_type_id'	=> $af->type($type),
		]);
	}



	public static function uid($userid) {
		global $user;
		if (!empty($userid)) {
			if (tbx_array($userid)) return (int) $userid['user_id'];
			return (int) $userid;
		}
		if (!empty($user['user_id'])) return (int) $user['user_id'];
		return false;
	}

}
