<?php
class Config {
    public static $debug = false;
    
    public static function strToChr($str) {
        $ret = array();

        $bits = str_split($str);

        for ($x = 0; $x < count($bits); $x++) {
            $ret[] = ord($bits[$x]);
        }

        return $ret;
    }

    public static function waitForInput() {
        $input = '';

        $read = [STDIN];
        $write = null;
        $except = null;

        readline_callback_handler_install('', function() {});

        // Read characters from the command line one at a time until there aren't any more to read
        do {
            $input .= fgetc(STDIN);
        } while (stream_select($read, $write, $except, 0, 1));

        readline_callback_handler_remove();

        return $input;
    }

    public static function autoload($class) {
        $file = __DIR__ . "/" . str_replace('\\', '/', strtolower($class)) . ".inc";
        
        if (file_exists($file)) {
            require $file;
        } else {
            if (Config::$debug) {
                debug_print_backtrace();
            }
            
            die("Couldnt find file: $file\n");
        }
    }

    public static function randomWords($num = 3, $join = "") {
        $output = shell_exec("shuf -n$num /usr/share/dict/american-english");

        $words = preg_split("/\r\n|\n|\r/", $output);
        
        $ret = array();

        array_pop($words);
        
        foreach ($words as $word) {
            $bits = preg_split("/'/", $word);
            $word = strtolower($bits[0]);
            $ret[] = $word;
        }
        
        return implode($join, $ret);
    }

    public static function toTable($data) {
        $cols = count($data[0]);

        $charsPerCol = floor(Config::$screenWidth / $cols) - $cols;

        $rows = count($data);

        $ret = "";

        for ($row = 0; $row < $rows; $row++) {
            $r = $data[$row];

            for ($col = 0; $col < $cols; $col++) {
                if ($col > 0) {
                    $ret .= "|";
                }

                $c = $r[$col];

                if (strlen(Colors::stripColor($c)) > $charsPerCol) {
                    $ret .= substr($c, 0, $charsPerCol);
                } else if (strlen(Colors::stripColor($c)) == $charsPerCol) {
                    $ret .= $c;
                } else if (strlen(Colors::stripColor($c)) < $charsPerCol) {
                    $ret .= str_pad($c, $charsPerCol, " ");
                }
            }

            $ret .= "\n";
        }
        
        return $ret;
    }

    public static function drawTable($widths, $data) {
        $ret = array();
        
        // figure out the total numbers of lines
        $lines = 0;

        foreach ($data as $d) {
            if (count($d) > $lines) {
                $lines = count($d);
            }
        }

        for ($line = 0; $line <= $lines; $line++) {
            $curStr = "";

            foreach ($data as $count => $d) {
                if (isset($d[$line])) {
                    $str = Colors::stripColor($d[$line]);

                    if (Config::containsMb($str)) {
                        $diff = strlen($d[$line]) - strlen(utf8_decode($str));

                        if (strlen(utf8_decode($str)) <= $widths[$count]) {    
                            $print = str_pad($d[$line], $widths[$count] + $diff, " ") . "|";
                        } else {
                            $print = substr($d[$line], 0, $widths[$count] + $diff) . "|";
                        }
                    } else {
                        if (strlen($str) <= $widths[$count]) {
                            $c = $widths[$count] - strlen(utf8_decode($str));

                            $print = $d[$line];

                            for ($cc = 0; $cc < $c; $cc++) {
                                $print .= " ";
                            }

                            $print .= "|";
                        } else {
                            $diff = strlen($d[$line]) - strlen(utf8_decode($str));
                            
                            $print = substr($d[$line], 0, $widths[$count] + $diff) . "|";
                        }
                    }
                } else {
                    $print = str_pad(" ", $widths[$count]) . "|";
                }

                $curStr .= $print;
            }
            
            $ret[] = $curStr;
        }

        return $ret;
    }

    public static function containsMb($str) {
        return !mb_check_encoding($str, 'ASCII') && mb_check_encoding($str, 'UTF-8');
    }
}

spl_autoload_register("Config::autoload");

