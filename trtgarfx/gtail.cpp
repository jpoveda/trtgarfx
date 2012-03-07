#include "TRT/Straw.hpp"
#include "Tools/Stream.hpp"
#include <TApplication.h>
#include <TStyle.h>
#include <TMultiGraph.h>

#include "boost/filesystem.hpp"

const unsigned int SignalBins    = 64;
const unsigned int SignalBinDiv  = 256;

int main(int argc,char * argv[])
{
 try
  {
    InitRoot();
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    
//    TApplication application("App",&argc,argv);
  
    boost::filesystem::create_directory("Plots");

    Signals::Signal electronsignal;
    Signals::Signal ionsignal;

    Tools::ASCIIInStream f("tailfile.dat");
    f>>electronsignal>>ionsignal;
    f.Close();

    Signals::Signal signal;
    signal.AdjustSignalEnviroment(electronsignal);
    signal.Clear();
    signal.Add(electronsignal);
    signal.Add(ionsignal);
    std::cout<<"Integral:"<<electronsignal.IntegralAbove(-10000000)<<std::endl;
    
    TH1D * hion=ionsignal.H1D();
    TH1D * helectron=electronsignal.H1D();
    TH1D * hsignal=signal.H1D();
    
    TF1 * tailfit=new TF1("tailfit","[0]*exp([1]*(x-20.5))+[2]*exp([3]*(x-20.5))",20.5,200);
    tailfit->SetParameter(0,-4.9645E-02);
    tailfit->SetParameter(1,-3.85286E-03);
    tailfit->SetParameter(2,-4.42067E-02);
    tailfit->SetParameter(3,-2.17964E-02);
    TF1 * gaussfit=new TF1("mygauss","[0]*exp([1]*(x-20.5)^2)",0,200);
    gaussfit->SetParameter(0,-0.18);
    gaussfit->SetParameter(1,-0.12);
    Signals::Signal gauss(3.125/SignalBinDiv);
    gauss.SetSignalLength(3.125*SignalBins);
    gauss.Set(gaussfit);
    Signals::Signal exp(3.125/SignalBinDiv);
    exp.SetSignalLength(3.125*SignalBins);
    exp.Clear();
    // Add Exponential component to each "part" of the "electron" gauss
    double gaussint=gauss.IntegralBelow(0);
    for(unsigned int i=0;i<gauss.GetSignalBins();++i)
    {
      exp.AddExp(gauss.GetSignalBin(i)*(-4.9645E-02/gaussint),-3.85286E-03,i*gauss.GetSignalBinTime(),200);
      exp.AddExp(gauss.GetSignalBin(i)*(-4.42067E-02/gaussint),-2.17964E-02,i*gauss.GetSignalBinTime(),200);
    }
    Signals::Signal res(3.125/SignalBinDiv);
    res.SetSignalLength(3.125*SignalBins);
    res.Clear();
    res.Add(gauss);
    res.Add(exp);
    std::cout<<"Int"<<res.IntegralBelow(0)<<std::endl;

    TCanvas * canvas=new TCanvas;
    TGraph * g=gauss.Graph();
    g->GetYaxis()->SetTitle("fC/ns");
    g->Draw("AL");
    canvas->Update();
    canvas->Print("fitgauss.pdf");
    g=exp.Graph();
    g->GetYaxis()->SetTitle("fC/ns");
    g->Draw("AL");
    canvas->Update();
    canvas->Print("fitexp.pdf");
    g=res.Graph();
    g->GetYaxis()->SetTitle("fC/ns");
    g->Draw("AL");
    canvas->Update();
    canvas->Print("fixres.pdf");
    
//    application.Run(kTRUE);
   
  }
  catch(const char * e)
  {
    std::cout<<e<<std::endl;
    throw;
  }
  return 0;
}
