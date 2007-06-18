
/*
 Copyright (C) 2006, 2007 Ferdinando Ametrano
 Copyright (C) 2007 Chiara Fornarola
 Copyright (C) 2006, 2007 Marco Bianchetti
 Copyright (C) 2006, 2007 Cristina Duminuco
 Copyright (C) 2006, 2007 Giorgio Facchinetti
 Copyright (C) 2007 Katiuscia Manzoni

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#if defined(HAVE_CONFIG_H)     // Dynamically created by configure
    #include <qlo/config.hpp>
#endif

#include <qlo/correlation.hpp>
#include <ql/legacy/libormarketmodels/lmlinexpcorrmodel.hpp>
#include <ql/models/marketmodels/correlations/cotswapfromfwdcorrelation.hpp>
#include <ql/models/marketmodels/correlations/timehomogeneousforwardcorrelation.hpp>
#include <ql/models/marketmodels/correlations/expcorrelations.hpp>
#include <ql/models/marketmodels/historicalforwardratesanalysis.hpp>
#include <ql/math/statistics/sequencestatistics.hpp>

namespace QuantLibAddin {
      
    //Correlation model
    LmLinearExponentialCorrelationModel::LmLinearExponentialCorrelationModel(
        QuantLib::Size size,
        QuantLib::Real rho,
        QuantLib::Real beta,
        QuantLib::Size factors) {

            libraryObject_ = boost::shared_ptr<QuantLib::LmLinearExponentialCorrelationModel>(
                new QuantLib::LmLinearExponentialCorrelationModel(size,rho,beta,factors));

    }
    
    TimeHomogeneousForwardCorrelation::TimeHomogeneousForwardCorrelation(
           const QuantLib::Matrix& fwdCorrelation,
           const std::vector<QuantLib::Time>& rateTimes)
    {
        QL_REQUIRE(!rateTimes.empty(), "rate times vector is empty!");
        libraryObject_ =
            boost::shared_ptr<QuantLib::TimeHomogeneousForwardCorrelation>(
                new QuantLib::TimeHomogeneousForwardCorrelation(
                    fwdCorrelation, rateTimes));
    }

    ExponentialForwardCorrelation::ExponentialForwardCorrelation(
                                const std::vector<QuantLib::Time>& rateTimes,
                                QuantLib::Real longTermCorr,
                                QuantLib::Real beta,
                                QuantLib::Real gamma,
                                const std::vector<QuantLib::Time>& times) {
        QL_REQUIRE(!rateTimes.empty(), "rate times vector is empty!");
        libraryObject_ =
            boost::shared_ptr<QuantLib::ExponentialForwardCorrelation>(new
                QuantLib::ExponentialForwardCorrelation(rateTimes,
                                                        longTermCorr,
                                                        beta,
                                                        gamma,
                                                        times));
    }

    CotSwapFromFwdCorrelation::CotSwapFromFwdCorrelation(
            const boost::shared_ptr<
                            QuantLib::PiecewiseConstantCorrelation>& fwdCorr,
            const QuantLib::CurveState& curveState,
            QuantLib::Real displacement) {
        libraryObject_ =
            boost::shared_ptr<QuantLib::CotSwapFromFwdCorrelation>(new
                QuantLib::CotSwapFromFwdCorrelation(fwdCorr,
                                                    curveState,
                                                    displacement));
    }
  
    HistoricalForwardRatesAnalysis::HistoricalForwardRatesAnalysis(
        const boost::shared_ptr<QuantLib::SequenceStatistics>& stats,
        const QuantLib::Date& startDate,
        const QuantLib::Date& endDate,
        const QuantLib::Period& step,
        const boost::shared_ptr<QuantLib::InterestRateIndex>& fwdIndex,
        const QuantLib::Period& initialGap,
        const QuantLib::Period& horizon,
        const std::vector<boost::shared_ptr<QuantLib::IborIndex> >& iborInd,
        const std::vector<boost::shared_ptr<QuantLib::SwapIndex> >& swapInd,
        const QuantLib::DayCounter& yieldCurveDayCounter,
        QuantLib::Real yieldCurveAccuracy)
    {
        libraryObject_ =
            boost::shared_ptr<QuantLib::HistoricalForwardRatesAnalysis>(new
                QuantLib::HistoricalForwardRatesAnalysis(stats,
                                                         startDate,
                                                         endDate,
                                                         step,
                                                         fwdIndex,
                                                         initialGap,
                                                         horizon,
                                                         iborInd,
                                                         swapInd,
                                                         yieldCurveDayCounter,
                                                         yieldCurveAccuracy));
    }

    //QuantLib::Size HistoricalForwardRatesAnalysis::size() const {
    //    return libraryObject_->stats()->size();
    //}
    //QuantLib::Matrix HistoricalForwardRatesAnalysis::correlation() const {
    //    return libraryObject_->stats()->correlation();
    //}
    //QuantLib::Size HistoricalForwardRatesAnalysis::samples() const {
    //    return libraryObject_->stats()->samples();
    //}
    //std::vector<QuantLib::Real> HistoricalForwardRatesAnalysis::mean()const {
    //    return libraryObject_->stats()->mean();
    //}
    //std::vector<QuantLib::Real> HistoricalForwardRatesAnalysis::variance() const {
    //    return libraryObject_->stats()->variance();
    //}
    //std::vector<QuantLib::Real> HistoricalForwardRatesAnalysis::standardDeviation() const {
    //    return libraryObject_->stats()->standardDeviation();
    //}
    //std::vector<QuantLib::Real> HistoricalForwardRatesAnalysis::skewness() const {
    //    return libraryObject_->stats()->skewness();
    //}
    //std::vector<QuantLib::Real> HistoricalForwardRatesAnalysis::kurtosis() const {
    //    return libraryObject_->stats()->kurtosis();
    //}
    //std::vector<QuantLib::Real> HistoricalForwardRatesAnalysis::min() const {
    //    return libraryObject_->stats()->min();
    //}
    //std::vector<QuantLib::Real> HistoricalForwardRatesAnalysis::max() const {
    //    return libraryObject_->stats()->max();
    //}

}