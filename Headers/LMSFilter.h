//
// Created by pitersk on 29.07.18.
//

#ifndef RPIANC_LMSFILTER_H
#define RPIANC_LMSFILTER_H


#include "common.h"
#include "FIRFilter.h"

template<int filter_length>
class LMSFilter {
public:

    typedef std::array<sample_type, filter_length> samples_array;
    typedef std::array<double, filter_length> filter_coeffs_array;
    FIRFilter<filter_length> fir_filter;

    LMSFilter(float alpha_val) : _alpha{alpha_val}, _lms_coefficients{{0}}, _samples_buffer{{0}} {

    }

    virtual sample_type lms_step(sample_type x_reference_sample, sample_type error_sample) {
        static sample_type input_signal_power_estimate = 0.0;
        input_signal_power_estimate = (1 - _power_smoothing_factor) * input_signal_power_estimate +
                                      _power_smoothing_factor * error_sample * error_sample;
        // Shift samples buffer
        for (long int i = filter_length - 1; i >= 1; --i) {
            _samples_buffer[i] = _samples_buffer[i - 1];
        }
        _samples_buffer[0] = x_reference_sample;
        // Update filter coefficients
        lms_filter_update(
                -(_alpha / (input_signal_power_estimate + 0.1)) * static_cast<double>(error_sample));
        // Perform filtering step, to generate new y correction sample
        return fir_filter.fir_step(x_reference_sample);
    }

    void lms_filter_update(double update_step) {
        filter_coeffs_array filter_coeffs = fir_filter.get_coefficients();
        for (int i = 0; i < filter_length; ++i) {
            filter_coeffs.at(i) += _samples_buffer.at(i) * update_step;
        }
        fir_filter.set_coefficients(filter_coeffs);
    }

private:
    float _power_smoothing_factor = 0.001;
    float _alpha;
    filter_coeffs_array _lms_coefficients;
    samples_array _samples_buffer;
};


#endif //RPIANC_LMSFILTER_H
