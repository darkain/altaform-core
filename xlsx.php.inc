<?php


require_once('af_object_array.php.inc');




class af_xlsx extends af_object_array {


	////////////////////////////////////////////////////////////////////////////
	//CONSTRUCTOR
	////////////////////////////////////////////////////////////////////////////
	function __construct($type=false) {
		if (!empty($type)) $this->translate($type);
	}




	////////////////////////////////////////////////////////////////////////////
	//CALLED AFTER PARSE() - RETURN TRUE FOR VALID, FALSE FOR INVALID
	////////////////////////////////////////////////////////////////////////////
	public function validate() { return true; }




	////////////////////////////////////////////////////////////////////////////
	//CALLED AFTER VALIDATE() - GREAT FOR INHERITING THIS CLASS
	////////////////////////////////////////////////////////////////////////////
	public function process() {}




	////////////////////////////////////////////////////////////////////////////
	//SET THE TRANSLATION TYPE
	////////////////////////////////////////////////////////////////////////////
	public function translate($type) {
		$this->translate = $type;
	}




	////////////////////////////////////////////////////////////////////////////
	//PARSE THE XLSX/ZIP FILE
	////////////////////////////////////////////////////////////////////////////
	public function parse($file=false) {
		global $site;
		$site['debug'] = true;

		//OPEN XLSX/ZIP FILE
		if (!empty($file)) {
			if (!$this->_openfile($file)) return false;
		}

		//LOOP THROUGH EACH ROW AND READ CONTENTS
		foreach ($this->sheet->sheetData->row as $key => $value) {
			$row		= (int) $value->attributes()->r;
			$columns	= count($value->c);

			//FIRST ROW, HEADER!
			if ($row === 1) {
				$this->_header($value, $columns);
				continue;
			}

			//IF WE DON'T HAVE A HEADER ROW, WE CANNOT DO ANYTHING!
			if (empty($this->header)) continue;

			//BODY CONTENTS
			$this->_body($value, $columns);
		}

		if (!$this->validate()) {
			throw new afException('XLSX FAILED DATA VALIDATION');
			return false;
		}

		$this->process();
		return true;
	}




	////////////////////////////////////////////////////////////////////////////
	//READ THE STRING VALUE FROM THE XLSX STRINGS TABLE
	////////////////////////////////////////////////////////////////////////////
	protected function _string($id) {
		$item = $this->strings->si[(int)$id];

		if (!empty($item->t)) {
			return trim(preg_replace('/\s\s+/', ' ', $item->t));
		}

		if (!empty($item->r)) {
			$return = '';
			foreach ($item->r as $part) $return .= $part->t;
			return trim(preg_replace('/\s\s+/', ' ', $return));
		}

		return '';
	}




	////////////////////////////////////////////////////////////////////////////
	//RETURNS: STRING=NEW NAME - TRUE=IGNORE - FALSE=ERROR
	////////////////////////////////////////////////////////////////////////////
	protected function _translate($name) {
		global $db;

		if (empty($name)) return true;
		if (empty($this->translate)) return $name;

		$item = $db->cell([
			's' => 'pudl_translate',
			't' => 'pudl_translate_type'
		], 'string_new', [
			's.translate_type_id=t.translate_type_id',
			't.translate_type'	=> $this->translate,
			's.string_old'		=> $name,
		]);

		if ($item !== false) return (empty($item)) ? $name : $item;

		return false !== $db->cell([
			's' => 'pudl_translate',
			't' => 'pudl_translate_type'
		], 'string_new', [
			's.translate_type_id=t.translate_type_id',
			's.string_old'		=> $name,
		]);
	}




	////////////////////////////////////////////////////////////////////////////
	//OPEN ZIP FILE AND READ CONTENTS
	////////////////////////////////////////////////////////////////////////////
	protected function _openfile($filename) {
		$zip = zip_open($filename);

		if (!is_resource($zip)) {
			throw new afException('CANNOT OPEN XLSX FILE - ' . $filename);
			return;
		}

		while ($entry = zip_read($zip)) {
			$file = zip_entry_name($entry);

			if ($file === 'xl/sharedStrings.xml') {
				$this->strings = simplexml_load_string(
					zip_entry_read($entry, zip_entry_filesize($entry))
				);

			} else if ($file === 'xl/worksheets/sheet1.xml'  &&  empty($this->sheet)) {
				$this->sheet = simplexml_load_string(
					zip_entry_read($entry, zip_entry_filesize($entry))
				);
			}
		}

		zip_close($zip);

		if (empty($this->strings)) {
			throw new afException('CANNOT READ XLSX STRINGS - ' . $filename);
			return;
		}

		if (empty($this->sheet)) {
			throw new afException('CANNOT READ XLSX SHEET - ' . $filename);
			return;
		}

		return true;
	}




	////////////////////////////////////////////////////////////////////////////
	//PARSE THE FIRST ROW / HEADER ROW
	////////////////////////////////////////////////////////////////////////////
	protected function _header($row, $columns) {
		for ($i=0; $i<$columns; $i++) {
			$item = $row->c[$i];
			if (!isset($item->attributes()->r)) continue;
			if (!isset($item->v)) continue;
			$column	= preg_replace('/\d/', '', $item->attributes()->r);
			$name	= $this->_string($item->v);
			$header	= $this->_translate($name);
			if ($header === false) throw new afException('UNKNOWN HEADER: ' . $name);
			if (is_string($header)) $this->header[$column] = $header;
		}
	}




	////////////////////////////////////////////////////////////////////////////
	//PARSE THE BODY CONTENTS
	////////////////////////////////////////////////////////////////////////////
	protected function _body($row, $columns) {
		$data = [];

		for ($i=0; $i<$columns; $i++) {
			$item = $row->c[$i];

			if (empty($item->attributes()->r)) continue;

			$rownum	= preg_replace('/\D/', '', $item->attributes()->r);
			$column	= preg_replace('/\d/', '', $item->attributes()->r);
			$type	= $item->attributes()->t;
			$value	= $item->v;

			if (empty($this->header[$column])) continue;

			if (!is_null($type)  &&  !is_null($value)) {
				$data[$this->header[$column]] = $this->_string($value);
				continue;
			}

			if (!is_null($type)  ||  is_null($value)) continue;

			$value = trim((string)$value);

			if (strlen($value) < 1) continue;

			if (is_numeric($value)  &&  strpos($value, 'e') === false  &&  strpos($value, '.') > 0) {
				$data[$this->header[$column]] = round($value, 2);

			} else if (ctype_digit($value)) {
				$data[$this->header[$column]] = (int)$value;

			} else {
				$data[$this->header[$column]] = $value;
			}
		}

		//DID THIS ROW CONTAIN DATA? PUSH IT TO OUR ARRAY!
		if (!empty($data)) $this[$rownum] = $data;
	}



	protected $sheet		= [];
	protected $strings		= [];
	protected $header		= [];
	protected $translate	= false;

}
