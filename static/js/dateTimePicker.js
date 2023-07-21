// datepicker: select dates for the calendar in off-line mode
// timepicker: select time for the anomalous events
// author: Zhida Li
// last modified: July 18, 2023

$(document).ready(function () {
    // Datepickers
    $("#s_date").datepicker({
        dateFormat: 'yymmdd' // format of the date
    });

    $("#e_date").datepicker({
        dateFormat: 'yymmdd' // format of the date
    });

    $("#s_date_ano").datepicker({
        dateFormat: 'yymmdd' // format of the date
    });

    $("#e_date_ano").datepicker({
        dateFormat: 'yymmdd' // format of the date
    });

    // Timepickers
    $('#s_t_ano, #e_t_ano').timepicker({
        timeFormat: 'hhmm',
        controlType: 'select',
        oneLine: true
    });

    // Slider
    var slider = document.getElementById("cut_percentage");
    var trainPercentage = document.getElementById("train-percentage");
    var testPercentage = document.getElementById("test-percentage");

    // Set initial values
    trainPercentage.innerHTML = slider.value * 10;
    testPercentage.innerHTML = (10 - slider.value) * 10;

    slider.oninput = function () {
        trainPercentage.innerHTML = this.value * 10;
        testPercentage.innerHTML = (10 - this.value) * 10;
    }

});
