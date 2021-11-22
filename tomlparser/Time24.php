<?php

/**
 * Description of Time
 * This stores time as number of
 * days, so that time of day is a fraction
 * @author michael
 */

class Time24
{
    //put your code here
    public float $val = 0.0;
    
    const SEC_AUTO = 0;
    const SEC_INT = 1;
    const SEC_DEC = 2;
    const SEC_NONE = 4;


    public function __construct(?string $t24 = null) {
        if (!empty($t24)) {
            $this->val = daytime_str($t24);
        }
    }

        /**
     * Set conventional of day, modulus 24
     * @param int $hours 0-23
     * @param int $mins  0-59
     * @param int $seconds 0-59
     * @param double $fraction 0 - 0.999999
     */

    public function set(int $hours, int $mins, float $seconds) {
        $this->val = daytime_set($hours,$mins,$seconds);
        
    }
    /**
     * String assumed to be compatible with
     * regular expression
     * (\\d{2}:\\d{2}(:\\d{2})?(\\.\\d+)?)
     */
    public function setStr(string $t24) {
        $this->val = daytime_str($t24);
    }
    /**
     * Return 
     * @return string
     */
    public function format(int $flag = 0) : string {
        return daytime_format($this->val, $flag);
    }
    
    public function split(int &$h24, int &$min, float &$sec) {
        daytime_split($h24,$min,$sec);
    }
    public function __toString() {
        return $this->format();
    }
}


function time24_format(float $val, int $flags = 0) : string 
{
        $h24 = 0;
        $min = 0;
        $sec = 0.0;
        daytime_split($val,$h24,$min,$sec);
        switch($flags) {
            case DayTime::SEC_INT:
                return sprintf("%02d:%02d:%02.0f", $h24,$min,$sec);
            case DayTime::SEC_DEC:
                return sprintf("%02d:%02d:%08.6f",$h24,$min,$sec);
            case DayTime::SEC_NONE:
                return sprintf("%02d:%02d", $h24,$min);
            case DayTime::SEC_AUTO:
            default:
                if ($sec !== 0) {
                    if ($sec - floor($sec) > 0.0) {
                         return sprintf("%02d:%02d:%8.6f",$h24,$min,$sec);
                    }
                    else {
                        return sprintf("%02d:%02d:%02.0f",$h24,$min,$sec);
                    }
                }
                return sprintf("%02d:%02d", $h24,$min);
        }
}

function time24_str(string $t24) : float {
        $i = stripos($t24,":");
        if ($i === 2) {
            $hours = intval(substr($t24,0,2));
            $mins = intval(substr($t24,3,2));
        }
        else {
            throw new Exception("Invalid time string");
        }
        $i = stripos($t24,":",$i+1);
        if ($i == 5) {
            $secs = floatval(substr($t24,6));
        }
        else {
            $secs = 0;
        }
        return daytime_set($hours,$mins,$secs);
}

function time24_set(int $hours, int $mins, float $seconds) : float {
        if ($hours < 0 || $mins < 0 || $seconds < 0) {
            throw new Exception("DayTime rejects negative values");
        }
        $h24 = $hours % 24;
        $m60 = $mins % 60;
        $s60 = fmod($seconds,60.0);
        return ($h24 * 60.0 * 60.0 + $m60 * 60.0 + $s60) / (24.0*60*60.0);
}

function time24_split(float $val, int &$h24, int &$min, float &$sec) {
    $frs = $val * 24.0 * 60.0 * 60.0;
    $h24 = floor($frs / (60.0 * 60.0));
    $min =  floor($frs / 60.0) - $h24 * 60;
    $sec =  $frs - $h24*60*60 - $min*60;
}
