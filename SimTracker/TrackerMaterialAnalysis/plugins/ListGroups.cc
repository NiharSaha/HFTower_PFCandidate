#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <fmt/printf.h>

// ROOT
#include <TCanvas.h>
#include <TColor.h>
#include <TFrame.h>
#include <TLegend.h>
#include <TLegendEntry.h>
#include <TLine.h>
#include <TProfile2D.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TText.h>

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/Math/interface/Vector3D.h"
#include "DetectorDescription/Core/interface/DDCompactView.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "DetectorDescription/Core/interface/DDMaterial.h"
#include "FWCore/Framework/interface/ESTransientHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/types.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"

#include "MaterialAccountingGroup.h"
#include "TrackingMaterialPlotter.h"

static bool dddGetStringRaw(const DDFilteredView &view, const std::string &name, std::string &value) {
  std::vector<const DDsvalues_type *> result;
  view.specificsV(result);
  for (std::vector<const DDsvalues_type *>::iterator it = result.begin(); it != result.end(); ++it) {
    DDValue parameter(name);
    if (DDfetch(*it, parameter)) {
      if (parameter.strings().size() == 1) {
        value = parameter.strings().front();
        return true;
      } else {
        throw cms::Exception("Configuration") << " ERROR: multiple " << name << " tags encountered";
        return false;
      }
    }
  }
  return false;
}

/*
static inline
double dddGetDouble(const std::string & s, DDFilteredView const & view) {
  std::string value;
  if (dddGetStringRaw(view, s, value))
    return double(::atof(value.c_str()));
  else
    return NAN;
}
*/

static inline std::string dddGetString(const std::string &s, DDFilteredView const &view) {
  std::string value;
  if (dddGetStringRaw(view, s, value))
    return value;
  else
    return std::string();
}

class ListGroups : public edm::one::EDAnalyzer<> {
public:
  ListGroups(const edm::ParameterSet &);
  ~ListGroups() override;

private:
  void analyze(const edm::Event &, const edm::EventSetup &) override;
  void beginJob() override {}
  void endJob() override;
  void fillColor();
  void fillMaterialDifferences();
  void fillGradient();
  std::vector<std::pair<std::shared_ptr<TLine>, std::shared_ptr<TText> > > overlayEtaReferences();
  void produceAndSaveSummaryPlot(const edm::EventSetup &);

  const bool m_saveSummaryPlot;
  const edm::ESGetToken<DDCompactView, IdealGeometryRecord> ddToken_;
  std::vector<TH2F *> m_plots;
  std::set<std::string> m_group_names;
  std::vector<MaterialAccountingGroup *> m_groups;
  std::vector<unsigned int> m_color;
  std::vector<int> m_gradient;

  // The following maps are automatically filled by the script
  // dumpFullXML, when run with -c,--compare flag, and are injected in
  // this code via the header ListGroupsMaterialDifference.h, which is
  // included below. The first value of the pair in m_diff represents
  // the relative difference ((new - old)/old * 100, in %) of
  // radiation length, while the second referes to the energy loss (in
  // GeV/cm) changes. The values in m_values are ordered in the very
  // same way, ie. they contain the new values for radiation length
  // and energy loss, respectively.
  std::map<std::string, std::pair<float, float> > m_diff;
  std::map<std::string, std::pair<float, float> > m_values;
};

ListGroups::ListGroups(const edm::ParameterSet &iPSet)
    : m_saveSummaryPlot(iPSet.getUntrackedParameter<bool>("SaveSummaryPlot")),
      ddToken_(esConsumes<DDCompactView, IdealGeometryRecord>()) {
  m_plots.clear();
  m_groups.clear();
  TColor::InitializeColors();
  fillColor();
  fillMaterialDifferences();
  fillGradient();
}

ListGroups::~ListGroups() {
  for (auto plot : m_plots)
    delete plot;

  if (!m_groups.empty())
    for (auto g : m_groups)
      delete g;
}

#include "ListGroupsMaterialDifference.h"

void ListGroups::fillGradient(void) {
  m_gradient.reserve(200);
  unsigned int steps = 100;
  // if no index was given, find the highest used one and start from that plus one
  unsigned int index = ((TObjArray *)gROOT->GetListOfColors())->GetLast() + 1;

  float r1, g1, b1, r2, g2, b2;
  static_cast<TColor *>(gROOT->GetListOfColors()->At(kBlue + 1))->GetRGB(r1, g1, b1);
  static_cast<TColor *>(gROOT->GetListOfColors()->At(kAzure + 10))->GetRGB(r2, g2, b2);
  float delta_r = (r2 - r1) / (steps - 1);
  float delta_g = (g2 - g1) / (steps - 1);
  float delta_b = (b2 - b1) / (steps - 1);

  m_gradient.push_back(kBlue + 4);  // Underflow lowest bin
  unsigned int ii = 0;
  for (unsigned int i = 0; i < steps; ++i, ++ii) {
    new TColor(static_cast<Int_t>(index + ii), r1 + delta_r * i, g1 + delta_g * i, b1 + delta_b * i);
    m_gradient.push_back(index + ii);
  }

  m_gradient.push_back(kWhite);  // 0 level perfectly white

  static_cast<TColor *>(gROOT->GetListOfColors()->At(kOrange))->GetRGB(r1, g1, b1);
  static_cast<TColor *>(gROOT->GetListOfColors()->At(kOrange + 7))->GetRGB(r2, g2, b2);
  delta_r = (r2 - r1) / (steps - 1);
  delta_g = (g2 - g1) / (steps - 1);
  delta_b = (b2 - b1) / (steps - 1);
  for (unsigned int i = 0; i < steps; ++i, ++ii) {
    new TColor(static_cast<Int_t>(index + ii), r1 + delta_r * i, g1 + delta_g * i, b1 + delta_b * i);
    m_gradient.push_back(index + ii);
  }
  m_gradient.push_back(kRed);  // Overflow highest bin
}

void ListGroups::fillColor(void) {
  // With the introduction of the support for PhaseI and PhaseII detectors it
  // became quite difficult to maintain a list of colors that is in sync with
  // the real number of grouping used in the different scenarios. We therefore
  // define some reasonable set and loop over it in case the number of grouping
  // is larger than the number of colors.

  m_color.push_back(kBlack);  // unassigned

  m_color.push_back(kAzure);      // PixelBarrelLayer0_Z0
  m_color.push_back(kAzure - 1);  // PixelBarrelLayer0_Z20
  m_color.push_back(kAzure + 1);  // Layer1_Z0
  m_color.push_back(kAzure + 2);  // Layer1_Z20

  m_color.push_back(kGreen);       // EndCapDisk1_R0
  m_color.push_back(kGreen + 2);   // EndcapDisk1_R11
  m_color.push_back(kGreen + 4);   // EndcapDisk1_R7
  m_color.push_back(kSpring + 9);  // EndcapDisk2_R0
  m_color.push_back(kSpring + 4);  // EndcapDisk2_R7
  m_color.push_back(kSpring);      // EndcapDisk2_R7

  m_color.push_back(kRed);          // TECDisk0_R20
  m_color.push_back(kRed + 2);      // TECDisk0_R40
  m_color.push_back(kRed - 7);      // TECDisk0_R50
  m_color.push_back(kRed - 5);      // TECDisk0_R60
  m_color.push_back(kRed - 10);     // TECDisk0_R90
  m_color.push_back(kRed - 1);      // TECDisk1_Inner
  m_color.push_back(kRed - 2);      // TECDisk1_Outer
  m_color.push_back(kRed - 3);      // TECDisk1_R20
  m_color.push_back(kPink - 2);     // TECDisk2_Inner
  m_color.push_back(kPink - 3);     // TECDisk2_Outer
  m_color.push_back(kPink - 4);     // TECDisk2_R20
  m_color.push_back(kPink + 9);     // TECDisk3_Inner
  m_color.push_back(kPink + 8);     // TECDisk3_Outer
  m_color.push_back(kPink + 7);     // TECDisk3
  m_color.push_back(kMagenta - 2);  // TECDisk4_Inner
  m_color.push_back(kMagenta - 3);  // TECDisk4_Outer
  m_color.push_back(kMagenta - 4);  // TECDisk4_R33
  m_color.push_back(kMagenta - 5);  // TECDisk5_Inner
  m_color.push_back(kMagenta - 6);  // TECDisk5_Outer
  m_color.push_back(kMagenta - 7);  // TECDisk5_R33
  m_color.push_back(kRed);          // TECDisk6
  m_color.push_back(kMagenta - 9);  // TECDisk7_R40
  m_color.push_back(kViolet);       // TECDisk8

  m_color.push_back(kOrange + 9);   // TIBLayer0_Z0
  m_color.push_back(kOrange + 7);   // TIBLayer0_Z20
  m_color.push_back(kOrange + 5);   // TIBLayer0_Z40
  m_color.push_back(kOrange - 2);   // TIBLayer1_Z0
  m_color.push_back(kOrange - 3);   // TIBLayer1_Z30
  m_color.push_back(kOrange - 6);   // TIBLayer1_Z60
  m_color.push_back(kOrange + 4);   // TIBLayer2_Z0
  m_color.push_back(kOrange - 7);   // TIBLayer2_Z40
  m_color.push_back(kOrange);       // TIBLayer3_Z0
  m_color.push_back(kOrange + 10);  // TIBLayer3_Z50

  m_color.push_back(kViolet + 10);  // TIDDisk1_R0
  m_color.push_back(kViolet + 6);   // TIDDisk1_R30
  m_color.push_back(kViolet + 3);   // TIDDisk1_R40
  m_color.push_back(kViolet - 7);   // TIDDisk2_R25
  m_color.push_back(kViolet - 1);   // TIDDisk2_R30
  m_color.push_back(kViolet + 9);   // TIDDisk2_R40
  m_color.push_back(kViolet - 5);   // TIDDisk3_R24
  m_color.push_back(kViolet - 3);   // TIDDisk3_R30
  m_color.push_back(kViolet);       // TIDDisk3_R40

  m_color.push_back(kAzure);      // TOBLayer0_Z0
  m_color.push_back(kAzure + 8);  // TOBLayer0_Z20
  m_color.push_back(kAzure + 2);  // TOBLayer0_Z70
  m_color.push_back(kAzure + 4);  // TOBLayer0_Z80
  m_color.push_back(kCyan + 1);   // TOBLayer1_Z0
  m_color.push_back(kCyan - 9);   // TOBLayer1_Z20
  m_color.push_back(kCyan + 3);   // TOBLayer1_Z80
  m_color.push_back(kCyan + 4);   // TOBLayer1_Z90
  m_color.push_back(kAzure);      // TOBLayer2_Z0
  m_color.push_back(kAzure + 8);  // TOBLayer2_Z25
  m_color.push_back(kAzure + 2);  // TOBLayer2_Z80
  m_color.push_back(kAzure + 5);  // TOBLayer2_Z90
  m_color.push_back(kCyan + 1);   // TOBLayer3_Z0
  m_color.push_back(kCyan - 9);   // TOBLayer3_Z25
  m_color.push_back(kCyan + 3);   // TOBLayer3_Z80
  m_color.push_back(kCyan + 4);   // TOBLayer3_Z90
  m_color.push_back(kAzure);      // TOBLayer4_Z0
  m_color.push_back(kAzure + 8);  // TOBLayer4_Z25
  m_color.push_back(kAzure + 2);  // TOBLayer4_Z80
  m_color.push_back(kAzure + 5);  // TOBLayer4_Z90
  m_color.push_back(kCyan + 1);   // TOBLayer5_Z0
  m_color.push_back(kCyan - 9);   // TOBLayer5_Z25
  m_color.push_back(kCyan + 3);   // TOBLayer5_Z80
  m_color.push_back(kCyan + 4);   // TOBLayer5_Z90
}

std::vector<std::pair<std::shared_ptr<TLine>, std::shared_ptr<TText> > > ListGroups::overlayEtaReferences() {
  std::vector<std::pair<std::shared_ptr<TLine>, std::shared_ptr<TText> > > lines;

  lines.reserve(40);
  std::pair<float, float> deltaZ(293, 298);
  std::pair<float, float> deltaR(115, 118);
  float text_size = 0.033;

  for (float eta = 0.; eta <= 3.8; eta += 0.2) {
    float theta = 2. * atan(exp(-eta));
    if (eta >= 1.8) {
      lines.push_back(std::make_pair<std::shared_ptr<TLine>, std::shared_ptr<TText> >(
          std::make_shared<TLine>(deltaZ.first, deltaZ.first * tan(theta), deltaZ.second, deltaZ.second * tan(theta)),
          std::make_shared<TText>(deltaZ.first, deltaZ.first * tan(theta), fmt::sprintf("%2.1f", eta).c_str())));
      lines.back().second->SetTextFont(42);
      lines.back().second->SetTextSize(text_size);
      lines.back().second->SetTextAlign(33);
      lines.push_back(std::make_pair<std::shared_ptr<TLine>, std::shared_ptr<TText> >(
          std::make_shared<TLine>(-deltaZ.first, deltaZ.first * tan(theta), -deltaZ.second, deltaZ.second * tan(theta)),
          std::make_shared<TText>(-deltaZ.first, deltaZ.first * tan(theta), fmt::sprintf("-%2.1f", eta).c_str())));
      lines.back().second->SetTextFont(42);
      lines.back().second->SetTextSize(text_size);
      lines.back().second->SetTextAlign(13);
    } else {
      lines.push_back(std::make_pair<std::shared_ptr<TLine>, std::shared_ptr<TText> >(
          std::make_shared<TLine>(deltaR.first / tan(theta), deltaR.first, deltaR.second / tan(theta), deltaR.second),
          std::make_shared<TText>(deltaR.first / tan(theta), deltaR.first, fmt::sprintf("%2.1f", eta).c_str())));
      lines.back().second->SetTextFont(42);
      lines.back().second->SetTextSize(text_size);
      lines.back().second->SetTextAlign(23);
      if (eta != 0) {
        lines.push_back(std::make_pair<std::shared_ptr<TLine>, std::shared_ptr<TText> >(
            std::make_shared<TLine>(
                -deltaR.first / tan(theta), deltaR.first, -deltaR.second / tan(theta), deltaR.second),
            std::make_shared<TText>(-deltaR.first / tan(theta), deltaR.first, fmt::sprintf("-%2.1f", eta).c_str())));
        lines.back().second->SetTextFont(42);
        lines.back().second->SetTextSize(text_size);
        lines.back().second->SetTextAlign(23);
      }
    }
  }
  return lines;
}

void ListGroups::produceAndSaveSummaryPlot(const edm::EventSetup &setup) {
  const double scale = 10.;
  std::vector<TText *> nukem_text;
  static int markerStyles[10] = {kFullCircle,
                                 kFullSquare,
                                 kFullTriangleUp,
                                 kFullTriangleDown,
                                 kOpenCircle,
                                 kOpenSquare,
                                 kOpenTriangleUp,
                                 kOpenDiamond,
                                 kOpenCross,
                                 kFullStar};

  edm::ESTransientHandle<DDCompactView> hDdd = setup.getTransientHandle(ddToken_);

  for (const auto &n : m_group_names) {
    m_groups.push_back(new MaterialAccountingGroup(n, *hDdd));
  };

  std::unique_ptr<TCanvas> canvas(
      new TCanvas("Grouping_rz", "Grouping - RZ view", (int)(600 * scale * 1.25), (int)(120 * scale * 1.50)));
  canvas->GetFrame()->SetFillColor(kWhite);
  gStyle->SetOptStat(0);

  unsigned int color_index = 1;
  // Setup the legend
  std::unique_ptr<TLegend> leg(new TLegend(0.1, 0.1, 0.23, 0.34));
  leg->SetHeader("Tracker Material Grouping");
  leg->SetTextFont(42);
  leg->SetTextSize(0.008);
  leg->SetNColumns(3);
  std::unique_ptr<TProfile2D> radlen(
      new TProfile2D("OverallRadLen", "OverallRadLen", 600., -300., 300, 120., 0., 120.));
  std::unique_ptr<TProfile2D> eneloss(
      new TProfile2D("OverallEnergyLoss", "OverallEnergyLoss", 600., -300., 300, 120., 0., 120.));
  std::unique_ptr<TProfile2D> radlen_diff(
      new TProfile2D("OverallDifferencesRadLen", "OverallDifferencesRadLen", 600., -300., 300, 120., 0., 120.));
  std::unique_ptr<TProfile2D> eneloss_diff(
      new TProfile2D("OverallDifferencesEnergyLoss", "OverallDifferencesEnergyLoss", 600., -300., 300, 120., 0., 120.));

  for (auto g : m_groups) {
    m_plots.push_back(
        new TH2F(g->name().c_str(), g->name().c_str(), 6000., -300., 300, 1200., 0., 120.));  // 10x10 points per cm2
    TH2F &current = *m_plots.back();
    current.SetMarkerColor(m_color[color_index]);
    current.SetMarkerStyle(markerStyles[color_index % 10]);
    current.SetMarkerSize(0.8);
    current.SetLineWidth(1);
    for (const auto &element : g->elements()) {
      current.Fill(element.z(), element.perp());
      radlen->Fill(element.z(), element.perp(), m_values[g->name()].first);
      eneloss->Fill(element.z(), element.perp(), m_values[g->name()].second);
      radlen_diff->Fill(element.z(), element.perp(), m_diff[g->name()].first);
      eneloss_diff->Fill(element.z(), element.perp(), m_diff[g->name()].second);
    }

    if (color_index == 1)
      current.Draw();
    else
      current.Draw("SAME");

    leg->AddEntry(&current, g->name().c_str(), "lp")->SetTextColor(m_color[color_index]);
    color_index++;

    // Loop over the same chromatic scale in case the number of
    // allocated colors is not big enough.
    color_index = color_index % m_color.size();
  }
  leg->Draw();
  canvas->SaveAs("Grouping.png");

  std::vector<std::pair<std::shared_ptr<TLine>, std::shared_ptr<TText> > > lines = overlayEtaReferences();

  canvas->Clear();
  radlen->SetMinimum(0);
  radlen->SetMaximum(0.25);
  radlen->Draw("COLZ");
  for (const auto &line : lines) {
    line.first->SetLineWidth(5);
    line.first->Draw();
    line.second->Draw();
  }
  canvas->SaveAs("RadLenValues.png");

  canvas->Clear();
  eneloss->SetMinimum(0.00001);
  eneloss->SetMaximum(0.0005);
  eneloss->Draw("COLZ");
  for (const auto &line : lines) {
    line.first->SetLineWidth(5);
    line.first->Draw();
    line.second->Draw();
  }
  canvas->SaveAs("EnergyLossValues.png");

  canvas->Clear();
  gStyle->SetPalette(m_gradient.size(), &m_gradient.front());
  gStyle->SetNumberContours(m_gradient.size());
  radlen_diff->SetMinimum(-100);
  radlen_diff->SetMaximum(100);
  radlen_diff->Draw("COLZ");
  for (const auto &line : lines) {
    line.first->SetLineWidth(5);
    line.first->Draw();
    line.second->Draw();
  }
  canvas->SaveAs("RadLenChanges.png");

  canvas->Clear();
  eneloss_diff->SetMinimum(-100);
  eneloss_diff->SetMaximum(100);
  eneloss_diff->Draw("COLZ");
  for (const auto &line : lines) {
    line.first->SetLineWidth(5);
    line.first->Draw();
    line.second->Draw();
  }
  canvas->SaveAs("EnergyLossChanges.png");

  for (auto g : nukem_text)
    delete g;
}

void ListGroups::analyze(const edm::Event &evt, const edm::EventSetup &setup) {
  edm::ESTransientHandle<DDCompactView> hDdd = setup.getTransientHandle(ddToken_);

  DDSpecificsHasNamedValueFilter filter{"TrackingMaterialGroup"};
  DDFilteredView fv(*hDdd, filter);

  while (fv.next()) {
    // print the group name and full hierarchy of all items
    std::cout << dddGetString("TrackingMaterialGroup", fv) << '\t';
    m_group_names.insert(dddGetString("TrackingMaterialGroup", fv));

    // start from 2 to skip the leading /OCMS[0]/CMSE[1] part
    const DDGeoHistory &history = fv.geoHistory();
    std::cout << '/';
    for (unsigned int h = 2; h < history.size(); ++h)
      std::cout << '/' << history[h].logicalPart().name().name() << '[' << history[h].copyno() << ']';

    // DD3Vector and DDTranslation are the same type as math::XYZVector
    math::XYZVector position = fv.translation() / 10.;  // mm -> cm
    std::cout << "\t(" << position.x() << ", " << position.y() << ", " << position.z() << ") "
              << "[rho] " << position.Rho() << std::endl;
  };
  std::cout << std::endl;

  if (m_saveSummaryPlot)
    produceAndSaveSummaryPlot(setup);
}

void ListGroups::endJob() {}

//-------------------------------------------------------------------------
// define as a plugin
#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(ListGroups);
