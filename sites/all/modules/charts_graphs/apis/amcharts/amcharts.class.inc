<?php

class ChartsAmcharts extends ChartCanvas {

function set_data($rows, $x_labels) {
    
    $this->series = $rows;    
    $this->x_labels = $x_labels;
}


function _preprocess_values() {
  if ( in_array($this->type, array('pie', 'donut'))) {
    return $this->_preprocess_values_pie();
  }
  else {
    return $this->_preprocess_values_not_pie();
  }
}

function _preprocess_values_pie() {

  $pie_data = array();
  $keys = array_keys($this->series);
  $first_key = $keys[0];
  $first_series = $this->series[$first_key];
  $idx = 0; $max_val = max($first_series); $max_idx = -1;
  if (is_array($first_series)) {
    foreach ($first_series as $val) {
      $pie_data[$this->x_labels[$idx]] = $val;
      if ($val == $max_val) {
        $max_idx = $idx;
      }
      $idx++;
    }
  }
  
  $out = '<pie>';
    if (is_array($pie_data)) {
      $idx=0;
      foreach ($pie_data as $label => $point) {
        if ($idx == $max_idx) { $pullout = " pull_out='true' "; } else { $pullout = ""; }
        $out .="<slice title='${label}' ${pullout} >${point}</slice>";
        $idx++;
      }
    }
  $out .= '</pie>';
  
  return $out;

}

function _preprocess_values_not_pie() {

  $out = '<chart><series>';
  if (is_array($this->x_labels)) {
    $i = 1;
    foreach ($this->x_labels as $label) {
      $out .= "<value xid='${i}'>${label}</value>";
      $i++;
    }
  }
  $out .= '</series><graphs>';		
  
  if (is_array($this->series)) {
    foreach ($this->series as $name => $graph) {
      $out .= "<graph title='${name}' bullet='round'>";
        if (is_array($graph)) {
          $j=1;
          foreach ($graph as $val) {
            $out .= "<value xid='${j}'>${val}</value>";
            $j++;
          }
        }
      $out .= "</graph>";
    }
  }
  
  $out .= '</graphs></chart>';		
  
  return $out;  
}

function get_chart() {
  
 $unique = chart_graphs_random_hash();
    
 switch ($this->type) {
   case 'column':
     $am_category = 'amcolumn';
     $am_type = 'column';
     break;
   case 'bar':
     $am_category = 'amcolumn';
     $am_type = 'bar';
     break;
   case 'line':
     $am_category = 'amline';
     $am_type = 'line';
     break;
   case 'pie':
     $am_category = 'ampie';
     $am_type = 'pie';
     break;
   case 'donut':
     $am_category = 'ampie';
     $am_type = 'donut';
     break;
     
 }
 
 $color = $this->colour;
 $width = $this->width;
 $height = $this->height;
 
 $data = $this->_preprocess_values();
 
 $swfobj_mod_path = drupal_get_path('module', 'swfobject_api');
 $swfobj_js = $swfobj_mod_path . '/swfobject.js';
 //$chart_div_id = "drp_charts_graphs_" . $this->getUnique_ID();  
 drupal_add_js( $swfobj_js,1 );
  
  
 // AMCharts comes with swfobject.js whether you want it or not, so we might as well use theirs. 
 $mod_path = drupal_get_path('module', $this->getModuleName());
 $swf_uri = base_path() . $mod_path; // . '/' . $am_category;

 //$expressInst =  base_path() . $swfobj_mod_path . '/expressInstall.swf'; 

//dpm($this);

$settings = "<settings>
    <type>${am_type}</type>
    <depth>5</depth>
    <background><color>${color}</color>
    <alpha>80</alpha></background>
    <js_enabled>false</js_enabled>  
    <redraw>true</redraw> 
    <data_labels>
      <show><![CDATA[{title}: {value}%]]></show>
      <line_color>#FFFFFF</line_color>    
      <line_alpha>40</line_alpha>
    </data_labels>            
    <pie>
      <inner_radius>20%</inner_radius>
      <height>20</height>
      <angle>30</angle>
      <hover_brightness>-20</hover_brightness>
      <gradient>linear</gradient>
      <gradient_ratio>-10,60</gradient_ratio>
    </pie>
  </settings>";
$settings = str_replace("\n", "", $settings);
$settings = preg_replace('/>\s+?</im', '><', $settings);
$wmode = $this->get_wmode();
$chart_div_id = "drp_amcharts_" . $this->getUnique_ID(); 

return <<<HTML

  <script type="text/javascript" src="${swf_uri}/downloaded/swfobject.js"></script>
  
	<div id="${chart_div_id}">
			<strong>You need to upgrade your Flash Player</strong>
	</div>

	<script type="text/javascript">
		// <![CDATA[				
    var so = new SWFObject("${swf_uri}/downloaded/${am_category}.swf", "THIS IS TITLE", "${width}", "${height}", "8", "#FFFFFF");
    so.addVariable ("chart_settings", "${settings}");
    so.addVariable("preloader_color", "#999999");
    so.addVariable("chart_data", "${data}");
    so.addVariable("wmode", "${wmode}");
    so.write("${chart_div_id}");
    // ]]>
  </script>        
HTML;


}

}