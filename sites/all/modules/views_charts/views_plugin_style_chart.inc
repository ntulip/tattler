<?php


function theme_views_view_chart() {
  return "opa";
}

class views_plugin_style_chart extends views_plugin_style {
 /**
   * Set default options
   */
  function option_definition() {
    $options = parent::option_definition();

    $options['width'] = array('default' => '500');
    $options['height'] = array('default' => '700');    
    $options['type'] = array('default' => array('amcharts' => 'pie'));
    $options['wmode'] = 'window';  

    return $options;
  }

  /**
   * Options Form
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    unset( $form['grouping']);    
    
    $curr_disp = $this->view->current_display;
    $views_fields = $this->_get_fields();
    
    $def_val_series = $this->options['views_charts_series_fields'];
    $def_val_series = (empty($def_val_series) || !is_array($def_val_series)) ? array(t('-- None --')) : $def_val_series;
    
    $form['views_charts_series_fields'] = array(
      '#title' => t('Fields to be used in Chart Series'),
      '#type' => 'select',
      '#options' => $views_fields,
      '#multiple' => TRUE,
      '#required' => TRUE,
      '#description' => t('These fields will be used as data fields for chart series. Fields must contain numeric data!'),
      '#default_value' => $def_val_series,
    );

    $views_fields = $this->_get_fields();    
    $def_val_labels = $this->options['views_charts_x_labels'];
    $def_val_labels = (empty($def_val_labels)) ? array(t('-- None --')) : $def_val_labels;
    
    $form['views_charts_x_labels'] = array(
      '#title' => t('Fields to be used as X axis labels'),
      '#type' => 'select',
      '#options' => $views_fields,
      '#multiple' => FALSE,
      '#required' => TRUE,
      '#default_value' => $def_val_labels,
    );
    
    $form['width'] = array(
      '#type' => 'textfield',
      '#title' => t('Canvas width in pixels'),
      '#default_value' => $this->options['width'],
    );
    
    $form['height'] = array(
      '#type' => 'textfield',
      '#title' => t('Canvas height in pixels'),
      '#default_value' => $this->options['height'],
    );
    
    $engines = array();
    $types = array();
    
    foreach ($apis = chart_graphs_apis() as $api) {
      $engines[$api->name] = $api->nice_name;
      $types[$api->name] = $api->chart_types;
    }

    $form['engine'] = array(
      '#type' => 'select',
      '#title' => t('Charting Engine'),
      '#options' => $engines,
      '#default_value' => $this->options['engine']
    );
    
    foreach ($types as $engine => $type) {
      
      $default = !empty($this->options['type'][$engine]) ? $this->options['type'][$engine] : NULL;
      $hidden  = NULL;
      
      if ($this->options['engine'] != $engine) {
        $hidden = ' views_charts_chart_types_hidden';
      }
      
      $form['type'][$engine] = array(
        '#type' => 'radios',
        '#title' => t('Chart Type'),
        '#options' => $type,
        '#required' => TRUE,       
        '#default_value' => $default,
        '#prefix' => sprintf('<div class="views_charts_chart_types views_charts_%s_chart_types%s">', $engine, $hidden),
        '#suffix' => '</div>'
      );
    
    }
    
    $form['wmode'] = array(
      '#type' => 'select',
      '#title' => t('Window Mode (for flash charts only)'),
      '#options' => ChartCanvas::wmode_values(),
      '#default_value' => $this->options['wmode']
    );
    
    $form['y_min'] = array(
      '#type' => 'textfield',
      '#title' => t('Min value of Y Axis'),
      '#default_value' => ($this->options['y_min']) ? $this->options['y_min'] : 0,
    );
    
    $form['y_legend'] = array(
      '#type' => 'textfield',
      '#title' => t('Y Legend'),
      '#default_value' => $this->options['y_legend'],
    );

  }
  
  function render() {

    $db_rows = $this->view->result;
    $canvas = chart_graphs_get_graph($this->options['engine']);
    
    $cgp_data = $this->_transform_data();
    $canvas->set_data($cgp_data->rows, $cgp_data->x_labels);
    
    $curr_disp = $this->view->current_display;
    $title = $this->view->display[$curr_disp]->display_title;
    $canvas->title = $title;
    $canvas->type = $this->options['type'][$this->options['engine']];
    $canvas->y_legend = $this->options['y_legend'];
    $canvas->y_min = $this->options['y_min'];
    $canvas->height = $this->options['height'];
    $canvas->width = $this->options['width'];    
    //$canvas->colour = '#D54C78';
    $canvas->colour = '#000000';    
    $canvas->wmode = $this->options['wmode'];
    
    /*$canvas->set_data(
      array(
        'Some Value' => array(9,6,7,9,5,7,6,9,7),
        'Page Views' => array(6,7,9,5,7,6,9,7,3),
      ),
      array('one', 'two', 'three')
    );*/
    
    
    //'admin/build/views' - issues with javascript etc. Do not try to render
    if (arg(0) =='admin' && arg(1) == 'build' && arg(2) == 'views') {
      $msg = t("Preview not available for Charts display style. Please view on a full page");
      $msg = "<div class=\"messages error\"><b>$msg</b></div>";
      return $msg;
    }

    
    $out .= $canvas->get_chart();
    return $out;

  }
  
  /** Transform data from Views-centric representation
  * into standard charts_and_graphs input format **/
  function _transform_data() {
    
    $db_rows = $this->view->result;      
    
    $series_column_names = $this->options['views_charts_series_fields'];
    $x_label_column = $this->options['views_charts_x_labels'];      

    $views_fields = $this->view->field;

    $fields_x_names = array();    
    $series_full_names = $this->_get_fields(TRUE);
    $x_label_alias_found = FALSE;
    
    foreach ($views_fields as $idx => $f) {
      $db_alias = $f->field_alias;
      if (in_array($idx, $this->options['views_charts_series_fields'])) {
        $fields_x_names[$db_alias] = $series_full_names[$idx];
      }
      
      if ((trim($x_label_column) == trim($idx)) && $x_label_alias_found == FALSE ) {
        $x_label_column = $db_alias;
        $x_label_alias_found = TRUE;
      }
    }
    
    /* 
    * We need to re-map the db results array 
    *  so that labels are indexes **/
    $rows = array();
    $labels = array();
    foreach ($db_rows as $row ) {
      $cols = (array)$row;
      foreach ($cols as $key => $col ) {
        if ( trim($key) == trim($x_label_column) ) {
          $labels[] = $row->$key;
        }
        if (array_key_exists($key, $fields_x_names)) {
          $full_name = $fields_x_names[$key];
          $rows[$full_name][] = (float)$col;
        }        
      }
    }

    $ret = new stdClass();
    $ret->rows = $rows;
    $ret->x_labels = $labels; 
    
    return $ret;
  
  }
  
  /**
  * @param $return_pretty_name return only the label, so we can use it on chart
  *
  */
  function _get_fields($return_pretty_name = FALSE) {

    $handlers = $this->display->handler->get_handlers('field');
    
    $avail_fields = array();
    if (is_array($handlers)) {
      foreach ($handlers as $field => $handler) {
        if ($field == 'views_sql_groupedfields' ) continue; //skip the groupby utility field
  
          $field_alias = $handler->options['table'] . '_' . $handler->options['field'];
          $relationship = '';
  
            $all_relationships = $this->_get_relationships ();        
            $rel = $handler->options['relationship']; 
            $rel = $all_relationships[$rel];
            
            if (!empty($rel)) {
              $field_name = $rel->fieldprefix . '.' . $val->options['field']; // reserved, not used
              $field_alias = $rel->fieldprefix . '_' . $val->options['field'];          
              $relationship = (empty($rel)) ? '' : '[' . $rel->label . '] ';
            }
          
          $label = ($handler->label()) ? $handler->label(): $handler->ui_name();
        
          $field_name = $relationship . $handler->definition['group'] . ': ' .
                         $handler->definition['title'] . ' (' . $label . ')';
          
          
          if ($return_pretty_name) {
            $avail_fields[$field] = $label;
          }
          else {
            $avail_fields[$field] = $field_name;
          }
          
      }
    }
    
    return $avail_fields;
  }

  function _get_relationships() {
    $default_rels = array();
    $curr_disp_rels = array();
    
    $curr_disp = $this->view->current_display;
    
    $default_rels = $this->view->display['default']->handler->options['relationships'];
    $curr_displ_rels = $this->view->display[$curr_disp]->handler->options['relationships'];
    
    $default_rels = (is_array($default_rels)) ? $default_rels : array();
    $curr_displ_rels = (is_array($curr_displ_rels)) ? $curr_displ_rels : array();

    $relationships = array_merge($default_rels, $curr_displ_rels);

    $all_rels = array();    
    $base_table = $this->view->base_table;
    
    if (is_array($relationships)) {
        foreach ($relationships as $key => $val) {
            $obj = new stdClass();
            $obj->fieldprefix = $base_table . '_' . $val['table'];          
            $obj->label = $val['label'];
            $obj->table = $val['table'];
            $obj->field = $val['field'];                        
            $all_rels[$key] = $obj;
        }
    }

    return $all_rels;    
    
  }

  
}
  

