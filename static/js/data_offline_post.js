// Off-line: POST
// author: Zhida Li
// last modified: July 18, 2023

$(document).ready(function () {
    // Update the hidden input initial value
    var initial_val = $('#cut_percentage').val();
    var second_digit = 10 - initial_val;
    $('#cut_percentage_hidden').val(initial_val + '' + second_digit);

    // Update the hidden input value when the range slider changes
    // The 'change' event is fired after the value is committed by the user
    // and it's likely that the slider's value has been fully updated by the time this event fires.
    $('#cut_percentage').on('change', function () {
        var slider_val = $(this).val();
        var second_digit_update = 10 - slider_val;
        $('#cut_percentage_hidden').val(slider_val + '' + second_digit_update);
    });

    // Update the hidden input when a dropdown item is selected
    $('.dropdown-item4').click(function () {
        $('#rnn_seq_hidden').val($(this).text());
        $('#dropdownButton4').text('Sequence length selected: ' + $(this).text());
    });

    // Update the hidden input when a dropdown item is selected
    $('.dropdown-item4').click(function () {
        $('#rnn_seq_hidden').val($(this).text());
        // console.log($('#rnn_seq_hidden').val());
    });
});
