<?php


trait afPreference {


	public static function parsePreferences($data) {
		if (empty($data)) return [];
		return json_decode($data, true, 512, JSON_BIGINT_AS_STRING);
	}




	public function getPreferences() {
		global $db;

		if (!empty($this->_prefs)) return $this->_prefs;

		if (!empty($this->preference)) {
			return $this->_prefs = $this->parsePreferences($this->preference);
		}

		return $this->_prefs = $this->parsePreferences(
			$db->cellId('user_preference', 'preference', 'user_id', $this)
		);
	}




	public function getPreference($name, $default=false) {
		$prefs = $this->getPreferences();
		return isset($prefs[$name]) ? $prefs[$name] : $default;
	}




	public function hasPreference($name) {
		$prefs = $this->getPreferences();
		return isset($prefs[$name]);
	}




	public function setPreferences($preferences) {
		global $db;

		$this->_prefs		= $preferences;
		$this->preference	= json_encode($preferences);

		return $db->upsert('user_preference', [
			'user_id'		=> $this->id(),
			'preference'	=> $this->preference,
		]);
	}




	public function updatePreference($key, $value) {
		$prefs = $this->getPreferences();
		$prefs[$key] = $value;
		return $this->setPreferences($prefs);
	}



	private				$_prefs		= [];

}
