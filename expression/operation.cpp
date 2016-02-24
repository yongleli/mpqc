//
// Created by Chong Peng on 10/31/15.
//

#include "operation.h"

#include <boost/algorithm/string.hpp>


namespace mpqc{
    using Operations = mpqc::Operation::Operations;
    using Options = mpqc::Operation::Options;

    const std::unordered_map<std::wstring, Operations> Operation::one_body_operation = {
        {L"", Operations::Overlap},
        {L"T", Operations::Kinetic},
        {L"V", Operations::Nuclear}
    };

    const std::unordered_map<std::wstring, Operations> Operation::two_body_operation = {
        {L"G", Operations::Coulomb},
        {L"R", Operations::cGTG },
        {L"GR", Operations::cGTGCoulomb },
        {L"R2", Operations::cGTG2 },
        {L"dR2", Operations::DelcGTG2}
    };

    const std::map<Operations, std::wstring> Operation::operation_to_string = {
            {Operations::Overlap, L""},
            {Operations::Kinetic, L"T"},
            {Operations::Nuclear, L"V"},
            {Operations::Coulomb, L"G"},
            {Operations::cGTG, L"R"},
            {Operations::cGTG2, L"R2"},
            {Operations::cGTGCoulomb, L"GR" },
            {Operations::DelcGTG2, L"dR2"}
    };

    const std::unordered_map<std::wstring, Options> Operation::option = {
            {L"df", Options::DensityFitting},
    };

    bool Operation::is_onebody() const {

        for (auto it = one_body_operation.begin(); it != one_body_operation.end(); ++it ){
            if (it->second == operation_){
                return true;
            }
        }
        return false;
    }

    bool Operation::is_twobody() const {

        for (auto it = two_body_operation.begin(); it != two_body_operation.end(); ++it ){
            if (it->second == operation_){
                return true;
            }
        }
        return false;
    }

    bool Operation::is_r12() const {
        if(operation_ == Operations::cGTG){
            return true;
        }
        else if(operation_ == Operations::cGTG2){
            return true;
        }
        else if(operation_ == Operations::cGTGCoulomb){
            return true;
        }
        else if(operation_ == Operations::DelcGTG2){
            return true;
        }
        else{
            return false;
        }
    }

    bool Operation::has_option(Options op) const {

        auto df = std::find(options_.cbegin(),options_.cend(),op);
        return (df != options_.cend());
    }
    Operation::Operation(std::wstring oper, std::wstring opt) {

        // parse operation
        if (oper.empty()) {
            throw std::runtime_error("Empty Operation!. \n");
        }

        boost::trim(oper);

        auto iter1 = one_body_operation.find(oper);

        if (iter1 == one_body_operation.end()) {
            auto iter2 = two_body_operation.find(oper);

            if(iter2 != two_body_operation.end()){
                operation_ = iter2->second;
            }else{
                throw std::runtime_error("Invalid Operation");
            }
        }
        else{
            operation_ = iter1->second;
        }

        // parse option

        // split string by , and space
        if(!opt.empty()){
            std::vector<std::wstring> split_option;
            boost::split(split_option,opt,boost::is_any_of(L", "), boost::token_compress_on);

            std::vector<Options> result;

            for(const auto& op : split_option){
                auto iter = option.find(op);
                if(iter == option.end()){
                    throw std::runtime_error("Invalid Option");
                }
                else{
                    result.push_back(iter->second);
                }
            }
            options_ = result;
        }
    }

    bool Operation::operator==(const Operation &other) const {

        bool same_operation = (this->operation_ == other.operation_);

        bool same_option = false;

        if(options_.size() == other.options_.size()){
            same_option = std::equal(options_.begin(), options_.end(), other.options_.begin());
        }

        return same_operation && same_option;
    }

bool Operation::operator<(const Operation &other) const {
    return other.get_operation() == this->get_operation() ? this->get_options() < other.get_options() : this->get_operation() < other.get_operation();
}
}
