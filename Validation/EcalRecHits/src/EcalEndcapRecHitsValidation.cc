/*
 * \file EcalEndcapRecHitsValidation.cc
 *
 * \author C. Rovelli
 *
 */

#include "DQMServices/Core/interface/DQMStore.h"
#include <DataFormats/EcalDetId/interface/EEDetId.h>
#include <Validation/EcalRecHits/interface/EcalEndcapRecHitsValidation.h>

using namespace cms;
using namespace edm;
using namespace std;

EcalEndcapRecHitsValidation::EcalEndcapRecHitsValidation(const ParameterSet &ps) : ecalPeds(esConsumes()) {
  // ----------------------
  EEdigiCollection_token_ = consumes<EEDigiCollection>(ps.getParameter<edm::InputTag>("EEdigiCollection"));
  EEuncalibrechitCollection_token_ =
      consumes<EEUncalibratedRecHitCollection>(ps.getParameter<edm::InputTag>("EEuncalibrechitCollection"));

  // ----------------------
  // verbosity switch
  verbose_ = ps.getUntrackedParameter<bool>("verbose", false);

  // ----------------------
  meEEUncalibRecHitsOccupancyPlus_ = nullptr;
  meEEUncalibRecHitsOccupancyMinus_ = nullptr;
  meEEUncalibRecHitsAmplitude_ = nullptr;
  meEEUncalibRecHitsPedestal_ = nullptr;
  meEEUncalibRecHitsJitter_ = nullptr;
  meEEUncalibRecHitsChi2_ = nullptr;
  meEEUncalibRecHitMaxSampleRatio_ = nullptr;
  meEEUncalibRecHitsOccupancyPlusGt60adc_ = nullptr;
  meEEUncalibRecHitsOccupancyMinusGt60adc_ = nullptr;
  meEEUncalibRecHitsAmplitudeGt60adc_ = nullptr;
  meEEUncalibRecHitsPedestalGt60adc_ = nullptr;
  meEEUncalibRecHitsJitterGt60adc_ = nullptr;
  meEEUncalibRecHitsChi2Gt60adc_ = nullptr;
  meEEUncalibRecHitMaxSampleRatioGt60adc_ = nullptr;
  meEEUncalibRecHitsAmpFullMap_ = nullptr;
  meEEUncalibRecHitsPedFullMap_ = nullptr;
}

EcalEndcapRecHitsValidation::~EcalEndcapRecHitsValidation() {}

void EcalEndcapRecHitsValidation::bookHistograms(DQMStore::IBooker &ibooker,
                                                 edm::Run const &,
                                                 edm::EventSetup const &) {
  Char_t histo[200];

  ibooker.setCurrentFolder("EcalRecHitsV/EcalEndcapRecHitsTask");

  sprintf(histo, "EE+ Occupancy");
  meEEUncalibRecHitsOccupancyPlus_ = ibooker.book2D(histo, histo, 100, 0., 100., 100, 0., 100.);

  sprintf(histo, "EE- Occupancy");
  meEEUncalibRecHitsOccupancyMinus_ = ibooker.book2D(histo, histo, 100, 0., 100., 100, 0., 100.);

  sprintf(histo, "EE Amplitude");
  meEEUncalibRecHitsAmplitude_ = ibooker.book1D(histo, histo, 201, -20., 4000.);

  sprintf(histo, "EE Pedestal");
  meEEUncalibRecHitsPedestal_ = ibooker.book1D(histo, histo, 50, 190., 210.);

  sprintf(histo, "EE Jitter");
  meEEUncalibRecHitsJitter_ = ibooker.book1D(histo, histo, 100, 0., 100.);

  sprintf(histo, "EE Chi2");
  meEEUncalibRecHitsChi2_ = ibooker.book1D(histo, histo, 100, 18000., 22000.);

  sprintf(histo, "EE RecHit Max Sample Ratio");
  meEEUncalibRecHitMaxSampleRatio_ = ibooker.book1D(histo, histo, 120, 0.90, 1.05);

  sprintf(histo, "EE+ Occupancy gt 60 adc counts");
  meEEUncalibRecHitsOccupancyPlusGt60adc_ = ibooker.book2D(histo, histo, 100, 0., 100., 100, 0., 100.);

  sprintf(histo, "EE- Occupancy gt 60 adc counts");
  meEEUncalibRecHitsOccupancyMinusGt60adc_ = ibooker.book2D(histo, histo, 100, 0., 100., 100, 0., 100.);

  sprintf(histo, "EE Amplitude gt 60 adc counts");
  meEEUncalibRecHitsAmplitudeGt60adc_ = ibooker.book1D(histo, histo, 200, 0., 4000.);

  sprintf(histo, "EE Pedestal gt 60 adc counts");
  meEEUncalibRecHitsPedestalGt60adc_ = ibooker.book1D(histo, histo, 50, 190., 210.);

  sprintf(histo, "EE Jitter gt 60 adc counts");
  meEEUncalibRecHitsJitterGt60adc_ = ibooker.book1D(histo, histo, 100, 0., 100.);

  sprintf(histo, "EE Chi2 gt 60 adc counts");
  meEEUncalibRecHitsChi2Gt60adc_ = ibooker.book1D(histo, histo, 100, 18000., 22000.);

  sprintf(histo, "EE RecHit Max Sample Ratio gt 60 adc counts");
  meEEUncalibRecHitMaxSampleRatioGt60adc_ = ibooker.book1D(histo, histo, 120, 0.90, 1.05);

  sprintf(histo, "EE Amplitude Full Map");
  meEEUncalibRecHitsAmpFullMap_ = ibooker.bookProfile2D(histo, histo, 100, 0., 100., 100, 0., 100., 200, 0., 4000.);

  sprintf(histo, "EE Pedestal Full Map");
  meEEUncalibRecHitsPedFullMap_ = ibooker.bookProfile2D(histo, histo, 100, 0., 100., 100, 0., 100., 50, 194., 201.);
}

void EcalEndcapRecHitsValidation::analyze(const Event &e, const EventSetup &c) {
  const EEUncalibratedRecHitCollection *EEUncalibRecHit = nullptr;
  Handle<EEUncalibratedRecHitCollection> EcalUncalibRecHitEE;
  e.getByToken(EEuncalibrechitCollection_token_, EcalUncalibRecHitEE);
  if (EcalUncalibRecHitEE.isValid()) {
    EEUncalibRecHit = EcalUncalibRecHitEE.product();
  } else {
    return;
  }

  bool skipDigis = false;
  const EEDigiCollection *EEDigi = nullptr;
  Handle<EEDigiCollection> EcalDigiEE;
  e.getByToken(EEdigiCollection_token_, EcalDigiEE);
  if (EcalDigiEE.isValid()) {
    EEDigi = EcalDigiEE.product();
  } else {
    skipDigis = true;
  }

  // ----------------------
  // loop over UncalibRecHits
  for (EcalUncalibratedRecHitCollection::const_iterator uncalibRecHit = EEUncalibRecHit->begin();
       uncalibRecHit != EEUncalibRecHit->end();
       ++uncalibRecHit) {
    EEDetId EEid = EEDetId(uncalibRecHit->id());

    int mySide = EEid.zside();

    // general checks
    if (mySide > 0) {
      if (meEEUncalibRecHitsOccupancyPlus_)
        meEEUncalibRecHitsOccupancyPlus_->Fill(EEid.ix(), EEid.iy());
    }
    if (mySide < 0) {
      if (meEEUncalibRecHitsOccupancyMinus_)
        meEEUncalibRecHitsOccupancyMinus_->Fill(EEid.ix(), EEid.iy());
    }
    if (meEEUncalibRecHitsAmplitude_)
      meEEUncalibRecHitsAmplitude_->Fill(uncalibRecHit->amplitude());
    if (meEEUncalibRecHitsPedestal_)
      meEEUncalibRecHitsPedestal_->Fill(uncalibRecHit->pedestal());
    if (meEEUncalibRecHitsJitter_)
      meEEUncalibRecHitsJitter_->Fill(uncalibRecHit->jitter());
    if (meEEUncalibRecHitsChi2_)
      meEEUncalibRecHitsChi2_->Fill(uncalibRecHit->chi2());
    if (meEEUncalibRecHitsAmpFullMap_)
      meEEUncalibRecHitsAmpFullMap_->Fill(EEid.ix(), EEid.iy(), uncalibRecHit->amplitude());
    if (meEEUncalibRecHitsPedFullMap_)
      meEEUncalibRecHitsPedFullMap_->Fill(EEid.ix(), EEid.iy(), uncalibRecHit->pedestal());

    // general checks, with threshold at 60 ADC counts
    if (uncalibRecHit->amplitude() > 60) {
      if (mySide > 0) {
        if (meEEUncalibRecHitsOccupancyPlusGt60adc_)
          meEEUncalibRecHitsOccupancyPlusGt60adc_->Fill(EEid.ix(), EEid.iy());
      }
      if (mySide < 0) {
        if (meEEUncalibRecHitsOccupancyMinusGt60adc_)
          meEEUncalibRecHitsOccupancyMinusGt60adc_->Fill(EEid.ix(), EEid.iy());
      }
      if (meEEUncalibRecHitsAmplitudeGt60adc_)
        meEEUncalibRecHitsAmplitudeGt60adc_->Fill(uncalibRecHit->amplitude());
      if (meEEUncalibRecHitsPedestalGt60adc_)
        meEEUncalibRecHitsPedestalGt60adc_->Fill(uncalibRecHit->pedestal());
      if (meEEUncalibRecHitsJitterGt60adc_)
        meEEUncalibRecHitsJitterGt60adc_->Fill(uncalibRecHit->jitter());
      if (meEEUncalibRecHitsChi2Gt60adc_)
        meEEUncalibRecHitsChi2Gt60adc_->Fill(uncalibRecHit->chi2());
    }

    if (!skipDigis) {
      // Find the rechit corresponding digi
      EEDigiCollection::const_iterator myDigi = EEDigi->find(EEid);
      // int sMax    = -1; // UNUSED
      double eMax = 0.;
      if (myDigi != EEDigi->end()) {
        for (unsigned int sample = 0; sample < myDigi->size(); ++sample) {
          EcalMGPASample thisSample = (*myDigi)[sample];
          double analogSample = thisSample.adc();
          if (eMax < analogSample) {
            eMax = analogSample;
            // sMax = sample; // UNUSED
          }
        }
      } else
        continue;

      // ratio uncalibratedRecHit amplitude + ped / max energy digi
      const EcalPedestals *myped = &c.getData(ecalPeds);
      EcalPedestalsMap::const_iterator it = myped->getMap().find(EEid);
      if (it != myped->getMap().end()) {
        if (eMax > (*it).mean_x1 + 5 * (*it).rms_x1 && eMax != 0) {  // only real signal RecHit

          if (meEEUncalibRecHitMaxSampleRatio_) {
            meEEUncalibRecHitMaxSampleRatio_->Fill((uncalibRecHit->amplitude() + uncalibRecHit->pedestal()) / eMax);
          }

          if (meEEUncalibRecHitMaxSampleRatioGt60adc_ && (uncalibRecHit->amplitude() > 60)) {
            meEEUncalibRecHitMaxSampleRatioGt60adc_->Fill((uncalibRecHit->amplitude() + uncalibRecHit->pedestal()) /
                                                          eMax);
          }

          LogDebug("EcalRecHitsTaskInfo")
              << "endcap, eMax = " << eMax << " Amplitude = " << uncalibRecHit->amplitude() + uncalibRecHit->pedestal();
        } else
          continue;
      } else
        continue;
    }
  }  // loop over the UncalibratedRecHitCollection
}
