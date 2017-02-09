<?php


require_once('xlsx.php.inc');



class		af_csv
	extends	af_xlsx {




	////////////////////////////////////////////////////////////////////////////
	//CONSTRUCTOR
	////////////////////////////////////////////////////////////////////////////
	function __construct($type=false) {
		parent::__construct($type);
	}




	////////////////////////////////////////////////////////////////////////////
	//PARSE THE CSV FILE
	////////////////////////////////////////////////////////////////////////////
	public function parse($file=false) {
		$rownum	= 0;
		$handle	= @fopen($file, 'rb');


		//VERIFY FILE IS OKAY
		if (!is_resource($handle)) {
			throw new afException('CANNOT OPEN CSV FILE - ' . $file);
			return;
		}


		//PARSE HEADERS
		$this->header = fgetcsv($handle);


		//PARSE BODY
		while ($item = fgetcsv($handle)) {
			$rownum++;
			$data = [];

			foreach ($item as $key => $value) {
				if (!isset($this->header[$key])) continue;
				$data[$this->header[$key]] = $value;
			}

			if (!empty($data)) $this[] = $data;
		}

		fclose($handle);


		//PROCESS THE FILE
		return $this->_processing();
	}


}
