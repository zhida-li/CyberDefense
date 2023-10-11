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
    $('.dropdown-item3').click(function () {
        $('#algo_hidden').val($(this).text());
        $('#dropdownButton3').text('Machine learning algorithm selected: ' + $(this).text());
    });

    // Update the hidden input when a dropdown item is selected
    $('.dropdown-item3').click(function () {
        $('#algo_hidden').val($(this).text());
        // console.log($('#algo_hidden').val());
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

    // Show optional-content by default
    $('#optional-content').show();
    // Hide upload-section by default
    $('#upload-section').hide();

    // Event listener for changes to the radio buttons
    $('input[type=radio][name=site_choice]').change(function () {
        console.log("Radio button changed to: " + this.value);

        if (this.value === 'custom') {
            $('#optional-content').hide();
            $('#upload-section').show();
        } else {
            console.log("Default dataset selected");
            $('#optional-content').show();
            $('#upload-section').hide();
        }
    });
});
