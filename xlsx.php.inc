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
	public function process() { return true; }




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
		global $af;
		$af->config->debug = true;


		//OPEN XLSX/ZIP FILE
		if (!empty($file)) {
			if (!$this->_openfile($file)) return false;
		}


		//LOOP THROUGH EACH ROW AND READ CONTENTS
		foreach ($this->sheet->sheetData->row as $key => $value) {
			$row		= (int) $value->attributes()->r;
			$columns	= count($value->c);

			//IF TRANSLATE IS TRUE, WE USE COLUMN LETTERS, NOT HEADER ROW
			if ($this->translate !== true) {

				//FIRST ROW, HEADER!
				if ($row === 1) {
					$this->_header($value, $columns);
					continue;
				}

				//IF WE DON'T HAVE A HEADER ROW, WE CANNOT DO ANYTHING!
				if (empty($this->header)) continue;
			}

			//BODY CONTENTS
			$this->_body($value, $columns);
		}


		//VALIDATE CONTENTS OF FILE
		$valid = $this->validate();

		if (!empty($this->errors)) {
			throw new afException(
				"XLSX FAILED DATA VALIDATION\n" .
				implode("\n", $this->errors)
			);
			return false;
		}


		//PROCESS CONTENTS OF FILE
		return $valid ? $this->process() : false;
	}





	////////////////////////////////////////////////////////////////////////////
	//RETURNS: STRING=NEW NAME - TRUE=IGNORE - FALSE=ERROR
	////////////////////////////////////////////////////////////////////////////
	protected function _translate($name) {
		global $db;

		if ($name instanceof SimpleXMLElement) {
			$name = (string) $name;
		}

		if (!is_string($name)) {
			return $this->invalid('Invalid Name: ' . print_r($name,true));
		}

		if (empty($name)) return true;
		if (empty($this->translate)) return $name;

		if (is_array($this->translate)) {
			if (!isset($this->translate[$name])) return false;
			return $this->translate[$name];
		}

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
				$strings = simplexml_load_string(
					zip_entry_read($entry, zip_entry_filesize($entry)),
					'SimpleXMLElement',
					LIBXML_COMPACT|LIBXML_NOBLANKS|LIBXML_NOCDATA|LIBXML_NONET
				);

			} else if ($file === 'xl/worksheets/sheet1.xml'  &&  empty($this->sheet)) {
				$this->sheet = simplexml_load_string(
					zip_entry_read($entry, zip_entry_filesize($entry)),
					'SimpleXMLElement',
					LIBXML_COMPACT|LIBXML_NOBLANKS|LIBXML_NOCDATA|LIBXML_NONET
				);
			}
		}

		zip_close($zip);

		//VERIFY WE COULD READ THE MAIN SHEET DATA
		if (empty($this->sheet)) {
			throw new afException('CANNOT READ XLSX SHEET - ' . $filename);
			return;
		}

		//VERIFY WE COULD READ THE STRINGS LOOKUP TABLE
		if (empty($strings)  ||  empty($strings->si)) {
			throw new afException('CANNOT READ XLSX STRINGS - ' . $filename);
			return;
		}

		//OPTIMIZE STRINGS TABLE
		foreach ($strings->si as $item) {
			if (isset($item->t)) {
				$this->strings[] = afstring::doubletrim( (string) $item->t );

			} else if (isset($item->r)) {
				$string = '';
				foreach ($item->r as $part) $string .= $part->t;
				$this->strings[] = afstring::doubletrim($string);

			} else {
				$this->strings[] = '';
			}
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
			$name	= $this->strings[(int)$item->v];
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
			$attr = $item->attributes();

			if (empty($attr->r)) continue;

			$rownum	= (int) preg_replace('/\D/', '', $attr->r);
			$column	= preg_replace('/\d/', '', $attr->r);
			$type	= $attr->t;
			$value	= $item->v;


			//HEADER IS SAME AS COLUMN
			if ($this->translate === true) {
				$header = $column;

			//NO HEADER DATA FOUND
			} else if (empty($this->header[$column])) {
				continue;

			//HEADER FROM TRANSLATION TABLE
			} else {
				$header = $this->header[$column];
			}


			//PULL DATA FROM STRINGS TABLE
			if (!is_null($type)  &&  !is_null($value)) {
				$data[$header] = $this->strings[(int)$value];
				continue;
			}


			//EMPTY CELL, IGNORE IT
			if (!is_null($type)  ||  is_null($value)) continue;


			//VALUE IS SELF-CONTAINED, SO CLEAN IT UP
			$value = afstring::trim((string)$value);


			//EMPTY STRING (OR ALL WHITE-SPACE), MEANING EMPTY CELL, IGNORE IT
			if (strlen($value) < 1) continue;


			//FLOATING POINT VALUE
			//TODO: CUSTOMIZE ROUNDING PERCISION.
			if (is_numeric($value)  &&  strpos($value, 'e') === false  &&  strpos($value, '.') > 0) {
				$data[$header] = round($value, 2);


			//INTEGER VALUE
			} else if (ctype_digit($value)) {
				$data[$header] = (int)$value;


			//ALL OTHER TYPES, USUALLY TEXT STRINGS
			} else {
				$data[$header] = $value;
			}
		}

		//DID THIS ROW CONTAIN DATA? PUSH IT TO OUR ARRAY!
		if (!empty($data)) $this[$rownum] = $data;
	}




	////////////////////////////////////////////////////////////////////////////
	//STORE ERROR MESSAGE
	////////////////////////////////////////////////////////////////////////////
	protected function invalid($text, $row=0) {
		if ($row) $text = '• ROW: ' . $row . ' - ' . $text;
		$this->errors[] = $text;
		return false;
	}




	protected $sheet		= [];
	protected $strings		= [];
	protected $header		= [];
	protected $errors		= [];
	protected $translate	= false;

}
