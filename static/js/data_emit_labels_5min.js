//author: Zhida Li
// last modified: July 16, 2022
// task: feature selection btn

$(document).ready(function () {
    namespace = '/test_conn';

    var socket = io(namespace);
    var selectedOption1 = null;  // This will hold the selected dropdown item
    var selectedOption2 = null;
    var selectedOption3 = null;

    // Update selected option and button text when a dropdown item is clicked
    // For dropdown 1
    $('.dropdown-item1').click(function () {
        selectedOption1 = $(this).text();
        $('#dropdownButton1').text('Memory selected: ' + selectedOption1);
        $('#dropdownButton1').css('background-color', '#28a745');
    });

    // For dropdown 2
    $('.dropdown-item2').click(function () {
        selectedOption2 = $(this).text();

        if (selectedOption2 === 'GRU (GPU only)') {
            $('#dropdownButton2').text('ML model selected: ' + selectedOption2);
        } else {
            $('#dropdownButton2').text('Machine learning model selected: ' + selectedOption2);
        }

        $('#dropdownButton2').css('background-color', '#28a745');

        // Disable the "Number of most important features" dropdown if VFBLS or GRU is selected
        if (selectedOption2 === 'VFBLS' || selectedOption2 === 'GRU') {
            $('#dropdownButton3').prop('disabled', true);
        } else {
            $('#dropdownButton3').prop('disabled', false);
        }
    });

    // for dropdown 3
    $('.dropdown-item3').click(function () {
        selectedOption3 = $(this).text();
        $('#dropdownButton3').text('Number of most important features selected: ' + selectedOption3);
        $('#dropdownButton3').css('background-color', '#28a745');
    });

    // Emit selected option when "Connect" button is clicked
    // $('#btn_connect').click(function () {
    //     let data = {'selected_option1': selectedOption1};
    //     socket.emit('main_event', data);
    // });

    $('#btn_connect').click(function () {
        let data = {
            'selected_option1': selectedOption1,
            'selected_option2': selectedOption2,
            'selected_option3': selectedOption3
        };
        socket.emit('main_event', data);
    });
    // Start the background thread by pressing the "Connect button"
    // $('#btn_connect').click(function () {
    //     socket.emit('main_event');
    // });

    // Disconnect the real-time task (client -> server)
    $('form#form_disconnect').submit(function (event) {
        socket.emit('disconnect_request');
        return false;
    });

    // Server sends the confirmation msg to the client
    socket.on('disconnect_response', function (msg, cb) {
        $('#log_disconnect').append($('<span/>').text('Server received' + ': ' + msg.data).html());
        if (cb)
            cb();
    }); // end of disconnect_response


    // var socket = io.connect(location.protocol + '//' + document.domain + ':' + location.port + namespace);
    socket.on('server_response_text', function (res) {
        // console.log("Minute 0 result and local time (server ->):", res.data_results[0], res.data_t[0]);  // for debug
        var t_utc = res.data_t[0];
        $('#t_utc').text(t_utc);

        //document.getElementById("t1_num").innerHTML = t1; // both methods are fine
        var results_text0 = res.data_results[0];
        $('#results0_p').text(results_text0);

        // Flash the "Local Time" on webpage "Real-Time BGP Anomaly Detection" when "results0_p" is received
        $("#MyClockDisplay").css('color', 'red').animate({
            opacity: 0
        }, 200, function () {
            $(this).animate({
                opacity: 1
            }, 200);
        }).delay(500).queue(function (next) {
            $(this).css('color', 'black');
            next();
        });

        var results_text1 = res.data_results[1];
        $('#results1_p').text(results_text1);

        var results_text2 = res.data_results[2];
        $('#results2_p').text(results_text2);

        var results_text3 = res.data_results[3];
        $('#results3_p').text(results_text3);

        var results_text4 = res.data_results[4];
        $('#results4_p').text(results_text4);

    });

});
