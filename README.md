# Tutorial "Introduction to RooFit"

University of Trieste, October 2022

Dr. Michele Pinamonti 

---

## Table of contents
1. [Setup](#setup)
2. [Getting started](#getting_started)
3. [RooFit basics](#basics)
4. [Advanced examples](#advanced)
5. [Profile Likelihood fit and HistFactory](#histfactory)
  * [Getting started](#start)
  * [Simple workspace creation](#ws)
  * [Simple fit](#fit)
  * [Systematic Impact](#syst)
  * [Discovery significance](#sig)

---

## Setup <a name="setup"></a>

Using git:
  * connect to lxplus or INFN ts farm with ssh via terminal
  * clone the git project:
```
git clone https://github.com/pinamont/rootfit-tutorial-units.git RooFitTutorial
```
  * move to the `RooFitTutorial` directory
  * run the setup script:
```
source setup.sh
```

Alternatively, without cloning the git project:
  * connect to lxplus or INFN ts farm with ssh via terminal
  * create a directory `RooFitTutorial`
  * create a `setup.sh` text file inside the directory with the following content:
```
source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.20.04/x86_64-centos7-gcc48-opt/bin/thisroot.sh 
alias macro="root -l -b -q"
```
  * from inside the directory `RooFitTutorial` run the setup script:
```
source setup.sh
```


---

## Getting started <a name="getting_started"></a>


### ROOT Hello World

Let's start with some recap of some simple ROOT functionalities. 
Start from the file `HelloWorld_ROOT.C`, which contains these lines:
```C++
void HelloWorld_ROOT(){
    TCanvas *c = new TCanvas("c","c",800,600);
    
    TF1 *g = new TF1("g","gaus",-10,10);
    g->SetParameter(0,1);
    g->SetParameter(1,0.00027);
    g->SetParameter(2,5.2794);
    
    g->Draw();
    
    c->SaveAs("MyTest.png");
}
```
This simple code is doing the following:
  * creates a canvas, to contain all of our graphical objects
  * creates a function (class TF1), using the "gaus" pre-defined formula `[0]*exp(-pow(x-[1],2)/(2*[2]*[2]))`, in the range (-10,10)
  * sets the values of the parameters of this function:
    * parameter 0 is the normalization parameter
    * parameter 1 is the mean of the gaussian distribution
    * parameter 2 is its standard deviation
  * draws the function on the current canvas
  * saves the canvas to a image file.

Now let's try to do something more interesting. 
Please, notice that the exact place where to add the following lines in the code is left to the user. 
Create a histogram, by adding these lines:
```C++
TH1F *h = new TH1F("h","h",10,-10,10);
```
and fill it randomly 100 times, following the function we defined earlier:
```C++
h->FillRandom("g",100);
```
Then let's draw it, and make it nicer looking:
```C++
h->Draw("E");
h->SetMarkerStyle(kFullCircle);
h->SetMarkerSize(1.5);
```
You can try to re-run the code and see how the new saved file looks like (you need to keep the line that saves the canvas to the png file as last line in the code).
Do you see both the histogram and the function? 
Probably no. 
In order to see both, you need to add the drawing option `"same"` to the second one you draw. 
Also, you might need to re-adjust the range of the y-axis in order to see both the histogram and the function fully included in the canvas.
Example:
```C++
h->Draw("E");
g->Draw("same");
```
or
```C++
g->Draw();
h->Draw("E same");
g->GetHistogram()->SetMaximum(25);
```

Now let's move to the interesting part: let's make a "fit":
```C++
h->Fit(g,"R");
```


### RooFit Hello World

Now let's try to do the equivalent job with RooFit. 
Let's start with this code:
```C++
void HelloWorld_RooFit(){
    TCanvas *c = new TCanvas("c","c",800,600);
    
    RooRealVar x("x","Observable",-10,10);
    RooRealVar mean("mean","B0 mass",0.00027,-10,10,"GeV");
    RooRealVar sigma("sigma","B0 width",5.2794,0,10,"GeV");
    RooGaussian model("model","signal pdf",x,mean,sigma);
    
    RooPlot *xframe = x.frame();
    model.plotOn(xframe);
    
    xframe->Draw();
    c->SaveAs("HelloWorld.png");
}
```
Here we did the following:
  * create a canvas, as before
  * create 3 variables, as `RooRealVar` objects:
    * our observable "x"
    * the mean and the standard deviation of the gaussian function
  * create the function, which will be our "model", as a `RooGaussian` object (which is internally defined as a probability density function, i.e. a `RooAbsPdf`)
  * create a `RooPlot` object
  * plot the model in the `RooPlot` object
  * draw the `RooPlot` object in the canvas
  * save the canvas

Now, let's add the same items as before. 
Let's create a histogram.
In RooFit, given the model that we defined, this is as simple as this:
```C++
RooDataHist data(*model.generateBinned(x,100));
```
Notice that we didn't specify anywhere how many bins to have, so a default (and actually non-optimal) number is taken. 
The simplest way to do it, is to add this line before creating the binned dataset:
```C++
x.setBins(10);
```
To see the histogram (the binned dataset) together with the function (the model), just add the `RooDataHist` object we create to the `RooPlot` object:
```C++
data.plotOn(xframe);
```

Let's move to the fit part.
The simplest way to do a fit of this PDF to the binned dataset is the following:
```C++
model.fitTo(data);
```


---


## RooFit basics <a name="basics"></a>


### Performing the fit step by step

In the RooFit Hello World example above, we performed the fit with the simple method `fitTo()`. 

Alternatively, one can call the individual steps of the fit procedure individually:

  * create a negative-log-likelihood object, from the model applied to the data (remind: `RooAbsReal` is a generic "function" in the RooFit language):
```C++
RooAbsReal *nll = model.createNLL(data);
RooMinimizer m(*nll);
```
  * call the minimization process ("Migrad"):
```C++
m.migrad();
```
  * call a proper method to get the (approximate) uncertainties on the estimated parameters using estimated second derivatives at the NLL minimum ("Hesse"): 
```C++
m.hesse();
```
  * eventually call another function to get more accurate estimates of the uncertainties by scanning the Likelihood function, eventually catchning asymmetric errors ("Minos"):
```C++
m.minos();
```

(A bit of discussion on why one needs to call Hesse and Minos after Migrad can be found here: http://www.fresco.org.uk/minuit/cern/node32.html).
  
  
### Extended Likelihood

What is the main difference between the results of the two "Hello World" macros we ran? 

Of course, when using RooFit we got only 2 parameters out of the fit, while with a simple ROOT fit, we got 3... 

With RooFit we are missing the "normalization" parameter. 

In order to include it, we need to turn our maximum likelihood fit to an "extended" likelihood fit. 

One way to do it is the following:

  * declare a third parameter, describing the number of signal events:
```C++
RooRealVar nsig("nsig","N signal events",100,0,1000);
```
  * after declaring the "model", declare a `RooExtendPdf` object, taking as inputs the Gaussian model we used before and the new parameter we just declared:
```C++
RooExtendPdf extModel("extModel","signal extended model",model,nsig);
```
  * then we can re-do all the steps above, but of course replacing `model` with `modelExt`;

Do we get our normalization parameter result now? 

Which number do we get? 

Were we lucky to get from the fit exactly the same input number we injected when generating the toy data?

We actually told RooFit to generate a toy data-set of exactly 100 data points. 
We can tell him to instead fluctuate also the total number of generated events, which is what we actually expect to happen in real collision data when collecting a certain amount of integrated luminosity instead of saying "we collect N data events". 
To do it, let's add an option to the `generateBinned` method:
```C++
RooDataHist data(*extModel.generateBinned(x,100,RooFit::Extended(true)));
```

Now let's try to run the fit again, and look at the fitted value of `nsig`.


### Getting fit results

It is often useful to store the results of the fit in a proper `RooFit` objetc, a `RooFitResult`.
To do it, when performing the fit, one needs to add the option `Save(true)`, and the fit method will return a pointer to the `FitResult` object.
We can add to any of the previous macros (for instance the latest one) the following line:
```C++
RooFitResult *r = extModel.fitTo(data,RooFit::Save(true));
```
and then print the content of it at the end of the macro:
```C++
r->Print();
```

### Unbinned fits

Let's try to do the same as in the Hello-World but leaving the likelihood unbinned.

```C++
void HelloWorld_Unbinned(){
    
    RooRealVar x("x","Observable",-10,10);
    RooRealVar mean("mean","B0 mass",0.00027,-10,10,"GeV");
    RooRealVar sigma("sigma","B0 width",5.2794,0,10,"GeV");
    RooGaussian model("model","signal pdf",x,mean,sigma);
    
    TCanvas *c = new TCanvas("c","c",800,600);
    RooPlot *xframe = x.frame();
    
    RooDataSet data(*model.generate(x,100));
    data.plotOn(xframe);
    
    model.plotOn(xframe);
    
    model.fitTo(data);
    
    xframe->Draw();
    c->SaveAs("HelloWorld.png");
```

One can compare the results with the binned case.

One can also inspect the generated toy data one by one:
```C++
data.Print("V");
for(int i=0;i<data.numEntries();i++){
    data.get(i)->Print("V");
}
```


### Workspace and factory language

We can rewrite the same code that we used before in this way:
```C++
void HelloWorld_Factory(){
  RooWorkspace *w = new RooWorkspace("w");
  w->factory("Gaussian::model(x[-10,10],mean[0.00027,-10,10],sigma[5.2794,0,10])");
  w->var("x")->setBins(10);
  RooDataHist *data = w->pdf("model")->generateBinned(*w->var("x"),100);
  w->pdf("model")->fitTo(*data);
}
```

For completeness, this simple macro doesn't contain the graphical part, but you can re-add it as **exercise**.

One can then add lines to save the workspace with any additional RooFit content, and check content. 

```
w->import(*data,RooFit::Rename("data"));
w->writeToFile("ws.root");
```

After running the macro with these additional lines, try to open ROOT interactively with:
```bash
root -l ws.root
```
and type:
```C++
RooWorkspace *w = (RooWorkspace*)_file0->Get("w");
w->Print();
```


### PDF composition

Let's build a simple signal-plus-background model.
Take the code used already for the Hello-World example, but give to the Gaussian PDF a different name:
```C++
RooGaussian model_sig("model_sig","signal pdf",x,mean,sigma);
```
and add a new PDF for our background:
```C++
RooRealVar t("t","slope",-0.2,-10.,0.);
RooExponential model_bkg("model_bkg","background pdf",x,t);
```
Now declare a parameter for the fraction of signal:
```C++
RooRealVar s_frac("s_frac","signal fraction",0.5);
```
and finally create the composite model:
```C++
RooAddPdf model("model","model_sig+model_bkg",RooArgList(model_sig,model_bkg),RooArgList(s_frac));
```

**Exercise**: now try to fit it in the usual way. 
What do you get? 
Do you get fitted values also for the fractions of signal and background? 
How to fit the fraction of signal as well? 
(hint: what are the allowed ranges of the two `RooRealVar` defining the signal and background fractions?)

**Exercise 2**: turn this new model into a fully extended likelihood model, where the number of signal and background events are fitted and given as results of the fit, together with the PDF parameters.


---


## Advanced examples <a name="advanced"></a>


### A nicer example

Let's take what we learned and build a new example, with a couple of extra ingredients.

We will now create two macros, one for building the model and creating the pseudo-data and one for fitting. 

Let's create the first macro, `MyExample_CreateModel.C`:
```C++
void MyExample_CreateModel(){
    RooWorkspace *w = new RooWorkspace("w");
    
    RooRealVar x("x","Diphoton Invariant Mass",0,250.,"GeV");

    // signal model
    RooRealVar mH("mH","Resonance Mass",125,0,250,"GeV");
    RooRealVar sigma("sigma","Resonance width",10,5.,100,"GeV");
    RooGaussian model_sig("model_sig","Signal PDF",x,mH,sigma);
    
    // Background model
    RooRealVar t("t","Background slope",-0.01,-10.,0.);
    RooExponential model_bkg("model_bkg","Background pdf",x,t);
    
    // S+B model
    RooRealVar n_sig("n_sig","Signal yield",0,0,1e6);
    RooRealVar n_bkg("n_bkg","Background yield",10000,0,1e6);
    RooAddPdf model("model","model_sig+model_bkg",RooArgList(model_sig,model_bkg),RooArgList(n_sig,n_bkg));
    
    w->import(model);
    w->writeToFile("ws.root");
}
```
Now, as exercise, let the code also create two toy datasets, one with the background-only hypothesis (so by either only generating data from the background model, or generating it from the S+B model but fixing the number of signal events to zero),
and one with the signal+background hypothesis, with the number of signal events set to 500.

**Hints**: 
  * create unbinned datasets (class `RooDataSet`) with the method `generate` from the model you want;
  * use the `RooFit::Extended()` option in the `generate` method to Poisson-fluctuate also the number of events (no need to specify how many events: the number will be taken from the extended likelihood integral);
  * import the two created datasets in the same way as the model was imported (before calling the `writeToFile` method).

Now the first macro should be ready. 
Call it with `macro MyExample_CreateModel.C` and check that a file `ws.root` is created (you can also inspect it).

Then create a second macro:
```C++
void MyExample_FitModel(){
    
    // load the created workspace
    TFile *ws_file = new TFile("ws.root");
    RooWorkspace *w = (RooWorkspace*)ws_file->Get("w");
    
    // create a canvas and split it into two sub-canvases
    TCanvas *c = new TCanvas("c","c",1600,600);
    c->Divide(2);
    
    // set the binning of the observable (for visualization purposes only - data is unbinned)
    w->var("x")->setBins(50);
    
    // get the model
    RooAbsPdf *pdf = w->pdf("model");
    
    // initialize model settings and save a snapshot
    w->var("n_bkg")->setVal(1e4);
    w->var("n_sig")->setVal(0.);
    w->var("mH")->setVal(125);
    w->var("sigma")->setVal(10);
    w->saveSnapshot("SplusB_0",w->allVars());
}
```

Now as exercise:
  * create two `RooPlot` objects (called `xframe1` and `xframe2`), one to store the B-only data and fit, one for the S+B data and fit (from the same variable, as usual calling `w->var("x")->frame();`;
  * load the two datasets from the workspace (e.g. `w->data("data_Bonly")`) and plot each of them in the proper `RooPlot`;
  * plot each of the plots to a separate sub-canvas (hint: call `c->cd(1)` before calling `xframe1->Draw()` and `c->cd(1)` before calling `xframe2->Draw()`;
  * perform 2 fits:

1. the B-only data should be fitted with a B-only model, i.e. by fixing all the signal parameters (and its normalization to zero), e.g.:
```C++
    w->var("n_sig")->setVal(0.);
    w->var("n_sig")->setConstant(true);
    w->var("mH")->setConstant(true);
    w->var("sigma")->setConstant(true);
    pdf->fitTo(*w->data("data_Bonly"),RooFit::Extended());
```
  * don't forget the good practice to call `w->loadSnapshot("SplusB_0")` before setting these (NB: not *after*);
  * notice the option `RooFit::Extended()`, added to make sure the fit is performed in the "extended likelihood" mode (can try to remove it and see what happens);
2. the S+B data should be fitted with a S+B model, i.e. by leaving free-floating fixing all the signal parameters (hint: `w->loadSnapshot("SplusB_0");` could help);
  * the model after the B-only fit should be plotted to `xframe1` and the second fit to `xframe2`;

Finally save the canvas:
```C++
    c->SaveAs("myExample.png");
```

Additional cosmetics to make your plot nicer:
  * for each of the two `RooPlot` call something like this:
```C++
    xframe1->Draw();
    xframe1->SetTitle("Fit to B-only data");
    xframe1->SetMinimum(10);
    xframe1->SetMaximum(1e3);
    gPad->SetLogy();
```
  * can add a legend to each plot, generated automatically:
```C++
    std::unique_ptr<TLegend> leg1 = xframe1->BuildLegend();
    leg1->Draw();
```


### Histogram PDFs and categories

Now let's see an example on how to create a binned PDF, starting from a histogram, and how to split into categories, for instance signal and control samples/regions.

Let's start with a code like this:

```C++
void BinnedFit(){
    TCanvas *c = new TCanvas("c","c",1200,600);
    c->Divide(2);
    
    RooWorkspace *w = new RooWorkspace();
    
    // build histograms
    // signal in SR
    TH1F * h_sig_SR = new TH1F("h_sig_SR","h_sig_SR",4,0,4);
    // background in SR
    TH1F * h_bkg_SR = new TH1F("h_bkg_SR","h_bkg_SR",4,0,4);
    // signal in CR
    TH1F * h_sig_CR = new TH1F("h_sig_CR","h_sig_CR",4,0,4);
    // background in CR
    TH1F * h_bkg_CR = new TH1F("h_bkg_CR","h_bkg_CR",4,0,4);
    
    h_sig_SR->SetBinContent(1,0.1);
    h_sig_SR->SetBinContent(2,0.2);
    h_sig_SR->SetBinContent(3,0.3);
    h_sig_SR->SetBinContent(4,0.4);

    h_sig_CR->SetBinContent(1,0.01);
    h_sig_CR->SetBinContent(2,0.02);
    h_sig_CR->SetBinContent(3,0.03);
    h_sig_CR->SetBinContent(4,0.04);
    
    h_bkg_SR->SetBinContent(1,10.0);
    h_bkg_SR->SetBinContent(2,8.00);
    h_bkg_SR->SetBinContent(3,6.00);
    h_bkg_SR->SetBinContent(4,3.00);

    h_bkg_CR->SetBinContent(1,100.);
    h_bkg_CR->SetBinContent(2,90.0);
    h_bkg_CR->SetBinContent(3,80.0);
    h_bkg_CR->SetBinContent(4,70.0);
    
    RooRealVar x("x","x",0,4);
    x.setBins(4);
    
    RooDataHist hist_sig_SR("data_sig_SR","data_sig_SR",x,h_sig_SR);
    RooDataHist hist_sig_CR("data_sig_CR","data_sig_CR",x,h_sig_CR);
    RooDataHist hist_bkg_SR("data_bkg_SR","data_bkg_SR",x,h_bkg_SR);
    RooDataHist hist_bkg_CR("data_bkg_CR","data_bkg_CR",x,h_bkg_CR);
    
    RooHistPdf model_sig_SR("model_sig_SR","model_sig_SR",x,hist_sig_SR);
    RooHistPdf model_sig_CR("model_sig_CR","model_sig_CR",x,hist_sig_CR);
    RooHistPdf model_bkg_SR("model_bkg_SR","model_bkg_SR",x,hist_bkg_SR);
    RooHistPdf model_bkg_CR("model_bkg_CR","model_bkg_CR",x,hist_bkg_CR); 
}
```

Up to here we created 4 histograms (by hand), for signal and background in a control and a signal region.

Now we need to create a `RooCategory` variable (so an integer variable), to split between signal and control regions.

We can do it by adding:
```C++
    RooCategory channel("channel","channel");
    channel.defineType("CR", 0);
    channel.defineType("SR", 1);
```

Then, we want to create 2 models, one for the SR and one for the CR.
To to it (since they need to be extended models), we need to create 4 variables, for the number of events of signal and background in the 2 regions.
So let's add this line:
```C++
    RooRealVar n_sig_SR("n_sig_SR","n_sig_SR",1,0,1000);
```
and other corresponding ones for `n_bkg_SR`, `n_sig_CR` and `n_bkg_CR`.
Important: 
let's set the initial values of these parameters to sensible numbers, and the best is to set them to the integrals of the histograms we created before, so:
```C++
    n_sig_SR.setVal(h_sig_SR->Integral());
```
and corresponding lines for the other 3 parameters.

At this point we can create the extended models for SR and CR:
```C++
    RooAddPdf model_SR("model_SR","model_SR",RooArgList(model_sig_SR,model_bkg_SR),RooArgList(n_sig_SR,n_bkg_SR));
```
and you need to add of course a corresponding line for the model in the CR, called `model_CR`.

<!-- We can already try to perform fits, but first we need to create -->

Now, let's "link" together the two models, so that we can create a single model.
To do it, we will use the `RooCategory` variable we created before.
```C++
    RooSimultaneous model("model","model",channel);
    model.addPdf(model_SR,"SR");
```
and then we need to add a line to "add" also the CR model in the same way.

Let's move to the graphical part. 
Let's create 2 `RooPlot` objects, one for each region.
For each of them, just call the `frame` method on the same observable `x`, but of course use different names for the two plots, `xframe_SR` and `xframe_CR`.

Then you can add some cosmetics, e.g.:
```C++
    xframe_SR->SetTitle("SR");
    xframe_SR->GetYaxis()->SetTitle("Number of events");
```
and the same for the CR.

Next, let's plot the models to the frames:
```C++
    model_SR.plotOn(xframe_SR,RooFit::Normalization(1.,RooAbsReal::RelativeExpected));
```
Notice the `RooAbsReal::RelativeExpected`, which tells RooFit to use the exepected number of events as normalizatio for the (extended) PDF, instead of the number of data.

Then we should create our toy data:
```C++
    RooDataHist *data_SR = model_SR.generateBinned(x,RooFit::NumEvents(n_sig_SR.getVal()+n_bkg_SR.getVal()),RooFit::Extended());
    RooDataHist *data_CR = model_CR.generateBinned(x,RooFit::NumEvents(n_sig_CR.getVal()+n_bkg_CR.getVal()),RooFit::Extended());
```
and plot them (each on the proper `RooPlot`).

Then we should combine them into a single dataset:
```C++
    RooDataHist data("data","data",x, 
                    RooFit::Index(channel), 
                    RooFit::Import({{"SR", data_SR}, {"CR", data_CR}}));
```

At this point, we can save the plots as a snaphot of the "prefit" situation:
```C++
    c->cd(1);
    xframe_SR->Draw();
    c->cd(2);
    xframe_CR->Draw();
    c->SaveAs("Binned_preFit.png");
```


Now let's move to the fit. 
You can simply call `model.fitTo(data)` but you need to save the results into a `FitResult` object, called `r`.

What do we get?
Which parameters got fitted?
Is it useful to have a control region in this case?


The answer is actually "no".
Indeed, the signal and background yields are free-floating independently in the SR and in the CR, so in this way the CR is actually useless.
What we can do, is to link these parameters controlling the yield.
To do it, let's create other parameters, that will scale the signal and the background coherently in all the regions:
```C++
    RooRealVar mu_sig("mu_sig","mu_sig",1,-10,10);
    RooRealVar mu_bkg("mu_bkg","mu_bkg",1,-10,10);
```
And now, let's redefine the signal and background yields so that they are "N * mu", i.e.:
```C++
    RooFormulaVar mu_n_sig_SR("mu_n_sig_SR","mu_sig*n_sig_SR",RooArgList(mu_sig,n_sig_SR));
```
and other 3 lines for background and control region.

Now, let's modify the definition of the two original models in the two regions, so that they take these new parameters as normalizations for signal and background, so let's replace `n_sig_S` with `mu_n_sig_SR` and similarly all the other cases, in the definitions of `model_SR` and `model_CR`.

Finally, before re-doing the fit, set the original parameters like `n_sig_SR` to constant, leaving only the "mu" parameters free to float in the fit:
```C++
    n_sig_SR.setConstant(true);
```
and similarly for the other 3 parameters.

Now, let's redo the fit and look at the result.


To finish, so more cosmetics.

One can print the results of the fit in a cleaner way with:
```C++
    cout << endl;
    cout << "+-------------------------------+" << endl;
    cout << "|             RESULTS           |" << endl;
    cout << "+-------------------------------+" << endl;
    cout << "  mu(S) = " << mu_sig.getVal() << " +/- " << mu_sig.getError() << endl;
    cout << endl;
```

In terms of plots, one should look at the post-fit plots, e.g. by creating a new `RooPlot`:
```C++
    RooPlot *xframe_SR_postFit = x.frame();
    xframe_SR_postFit->SetTitle("SR (post-fit)");
    xframe_SR_postFit->GetYaxis()->SetTitle("Number of events");
    model_SR.plotOn(xframe_SR_postFit,
                    RooFit::Normalization(1.,RooAbsReal::RelativeExpected));
```
and the same for the CR.
Then one can save these post-fit plots to a new `TCanvas`, called for example "Binned_postFit.png".

Even more fancy, we can plot the background component in the plots individually:
```C++
    model_SR.plotOn(xframe_SR_postFit,RooFit::Components(model_bkg_SR),
                    RooFit::LineStyle(kDashed),RooFit::LineColor(kRed),
                    RooFit::Normalization(1.,RooAbsReal::RelativeExpected));
```
You can do it for all the plots, including the pre-fit ones, and see how plots look like.

Finally, for the post-fit plots only, one can even show the "post-fit error" on the model, by doing something like this:
```C++
    model_CR.plotOn(xframe_CR_postFit,
                    RooFit::VisualizeError(*r, 1),
                    RooFit::Normalization(1.,RooAbsReal::RelativeExpected)
                    );
```
Notice that this needs to be done **before** the model is plotted, so you should have something like this:
```C++
    model_CR.plotOn(xframe_CR_postFit,
                    RooFit::VisualizeError(*r, 1),
                    RooFit::Normalization(1.,RooAbsReal::RelativeExpected)
                    );
    model_CR.plotOn(xframe_CR_postFit,
                    RooFit::Normalization(1.,RooAbsReal::RelativeExpected));
    model_CR.plotOn(xframe_CR_postFit,RooFit::Components(model_bkg_CR),
                    RooFit::LineStyle(kDashed),RooFit::LineColor(kRed),
                    RooFit::Normalization(1.,RooAbsReal::RelativeExpected)
                    );
```


### Constraints and nuisance parameters

Let's move one step further and see how we can define constrained (nuisance) parameters in a likelihood.
Let's start with a simple model, like the one used for the first HelloWorld example:
```C++
void Example_NPs(){
    RooWorkspace *w = new RooWorkspace("w");
    
    TCanvas *c = new TCanvas("c","c",800,600);
    
    RooRealVar x("x","Observable",-10,10);
    RooRealVar mean("mean","B0 mass",0.00027,-10,10,"GeV");
    RooRealVar sigma("sigma","B0 width",5.2794,0,10,"GeV");
    RooGaussian model("model","signal pdf",x,mean,sigma);
    
    x.setBins(10);
    RooDataHist data(*model.generateBinned(x,100));
    
    w->import(data);
    w->import(model);
    
    w->saveSnapshot("Vanilla",w->allVars());
    
    RooFitResult *r = model.fitTo(data,RooFit::Save(),
                                  RooFit::PrintLevel(-1),RooFit::PrintEvalErrors(-1));
    r->Print();
}
```
(notice that we used the `RooFit::PrintLevel(-1)` and `RooFit::PrintEvalErrors(-1)` option to make the fit silent)

What we want to do now is to apply a Gaussian constraint to the parameter `sigma`, to represent the fact that we have some prior knowledge on it, for instance from a previous measurement or a theory prediction.

In RooFit, we have two possible ways to proceed:
  * we build a model that includes a Gaussian constrain term;
  * we add the constraint only when we perform the fit, as "external constraint".
  
Let's see how to proceed in the first way.
Note: we can add code at the end of the previous macro this time.
We need to create a new PDF, a Gaussian in this case, where the observable is this time not `x`, but `sigma`. 
We then set the mean of this Gaussian to the nominal value for `sigma` and its standard deviation to the uncertainty on the prior measurement / theory prediction for `sigma`:
```C++
    RooGaussian constr("constr", "constr", sigma, RooFit::RooConst(5.2794), RooFit::RooConst(0.2));
```

We can then build a new model, as the product of the previous plain model times the constraint term we just created:
```C++
    RooProdPdf modelc("modelc","signal pdf (with constraint)",RooArgSet(model,constr));
```

Let's then do, as **exercise**:
  * import the new model in the workspace;
  * load the previously saves snapshot;
  * perform a new fit, in the same way as before, by creating a new `FitResult` object, and printing the output. 

We can then rerun the code and compare the outputs of the first and the second fit.
Are the results different?
Are the uncertainties (on `sigma` and `mean`) changed?

Now let's give a look at the second method. 
Again we can safely add code at the end of what we have already.
Since we already created the constraint term, we can re-use it.
Now, as **exercise** will simply perform the fit with the *plain* model, and we will add the constraint term simply by adding the option `RooFit::ExternalConstraints(constr)` to the `fitTo` method.
Important: don't forget to load the snapshot before fitting!

We can now compare the results of the fits. 
Are we getting the same results?


---


## Profile Likelihood fit and HistFactory <a name="histfactory"></a>

This new section is about the usage of HistFactory.


### Getting started <a name="start"></a>

Let's first create two directories, to store the workspace and the xml steering files created by HistFactory:
```bash
mkdir ws
mkdir xml
```

Then let's generate the histograms.
To do it, download (or copy) the macro `CreateHistograms.C` and run it, with:
```bash
macro CreateHistograms.C
```

A directory `ExampleInputs` should have been created, with a number of root files inside.



---

### Simple workspace creation <a name="ws"></a>

We will create a simple example workspace based on histogram inputs (that we will create with a dummy ROOT macro).


Our likelihood model, and the meaning we give to it, is stored within a measurement - an HistFactory concept which needs to know:
  * how we want to nickname it;
  * where output files should be stored;
  * what's the parameter of interest (POI) of this measurement;
  * what are the parameters to be considered as a constant, if any - we typically include the default luminosity nuisance parameter created by HistFactory, called Lumi, within this "blacklist";
  * what are the default settings of the default luminosity parameter, used by HistFactory whenever you specify that a channel should be normalized by luminosity (see `SetNormalizeByTheory`).
  
We are also nice people who like to decouple logical steps, so we ask HistFactory to kindly not do anything else than exporting the workspace into a ROOT file (i.e. please HistFactory do not perform any statistical analysis without our consent).

Create the measurement object, set prefix for outputs, set parameter of interest (POI), 
set the "export-only" flag, with this sample macro:
```C++
using namespace RooFit;
using namespace RooStats;
using namespace HistFactory;

void CreateSimpleWS(){
    Measurement *meas = new Measurement("meas","meas");
    
    meas->SetOutputFilePrefix("ws/ws_minimal");
    meas->SetPOI("mu_signal");
    meas->SetExportOnly(true);
}
```

We want to remove the built-in "luminosity" uncertainty, so we set it to constant:
```C++
    meas->SetLumi(1.0);
    meas->SetLumiRelErr(1.0);
    meas->AddConstantParam("Lumi");
```

We then follow this logic:
  * we first create a channel (corresponding to some set of statistically-independent data);
  * we tell HistFactory where (meaning: in which file, under which subdirectory path and more specifically in which histogram) to find the data for this channel;
  * we then add the samples which contribute to this channel, specifying where to find their nominal histograms, and which normalisation-only (AddOverallSys) and also-shape uncertainties (AddHistoSys) should be considered (keeping in mind that variations of any kind which share the same name are correlated);
  * we also add free parameters to fit for determining the normalisation of our signal (and sometimes background) samples;
  * we add each sample to the channel.
  
Create the channel:
```C++
    Channel *chan_sr = new Channel("SR");
```

Set the data:
```C++
    chan_sr->SetData( "HTj", "ExampleInputs/data.root" );
```

Add signal sample, adding the POI as normalization factor to it:
```C++
    Sample *signal_sr = new Sample( "signal_sr", "HTj", "ExampleInputs/sig.root" );
    signal_sr->AddNormFactor( "mu_signal", 1, -10, 10 );
```

Add the background samples:
```C++
    Sample *bkg1_sr = new Sample( "bkg1_sr", "HTj", "ExampleInputs/bkg1.root" );
    Sample *bkg2_sr = new Sample( "bkg2_sr", "HTj", "ExampleInputs/bkg2.root" );
```
    
Assign some simple systematic uncertainties.
Cross-section uncertainties on the two background processes:
```C++
    bkg1_sr->AddOverallSys( "XS_bkg1",  0.90, 1.10 );
    bkg2_sr->AddOverallSys( "XS_bkg2",  0.70, 1.30 );
```
Then a shape uncertainty affecting the signal and one of the background samples:
```C++
    signal_sr->AddHistoSys( "JES", "HTj_jesDown","ExampleInputs/sig.root","",
                                   "HTj_jesUp","ExampleInputs/sig.root","");
    bkg1_sr->AddHistoSys( "JES", "HTj_jesDown","ExampleInputs/bkg1.root","",
                                 "HTj_jesUp","ExampleInputs/bkg1.root","");
```

Add samples to channel:
```C++
    chan_sr->AddSample( *signal_sr );
    chan_sr->AddSample( *bkg1_sr );
    chan_sr->AddSample( *bkg2_sr );
```
and channel to measurement:
```C++
    meas->AddChannel( *chan_sr );
```

At this point, we ask HistFactory to actually go and check the histograms, do its magic and create The Likelihood Model. We also persist this likelihood model in XML format, for our afternoons of debugging.
```C++
    meas->CollectHistograms();
    meas->PrintTree();
    meas->PrintXML("xml/", meas->GetOutputFilePrefix());
    MakeModelAndMeasurementFast(*meas);    
```

**Exercise**: add a control region.
Hists:
  * use similar commands as above, adding lines of code *before* calling `meas->CollectHistograms();`;
  * in general, can use all the same names, but substituting `_sr` with `_cr`;
  * we still need to assign the POI factor to the signal sample in the CR;
  * the input file names for the CR inputs are the same, but the histogram names have `"HTj_CR"` instead of `"HTj"` (also in the case of the JES systematic uncertainty - the histogram names are actually `HTj_CR_jesUp` and `HTj_CR_jesDown` in this case).
  


---

### Simple fit <a name="fit"></a>

This exercise shows how to perform a simple fit on the workspace produced earlier.

It also shows various ways of printing and saving the results of the fit.

Open the workspace file and extract the workspace object, 
extract the `ModelConfig` from the workspace, 
get the data we want to fit:
```C++
using namespace RooFit;
using namespace RooStats;
using namespace HistFactory;

void SimpleFit(){
    TFile *f = new TFile("ws/ws_minimal_combined_meas_model.root");
    RooWorkspace *w = (RooWorkspace*)f->Get("combined");
    ModelConfig *mc = (ModelConfig*)w->obj("ModelConfig");
    RooAbsData *dataset = w->data("obsData");
}
```

Now perform a S+B fit:
```C++
    w->var("mu_signal")->setVal(0);
    w->var("mu_signal")->setConstant(kFALSE);
    w->pdf("simPdf")->fitTo(*dataset);
```

From the output above, we can find the fitted values of all the parameters (of interest and nuisance) and their uncertainties.

Now, let's try to extract and print all and only the information we want, so that we could re-use it later:
  * the fitted value of the POI, with its error;
  * the fitted values of the NPs, with their errors (what we call the "pulls" and "constraints" of the NPs).
  
Let's extract the POI and print the fitted value and uncertainties (separately the up and down):
```C++
    RooRealVar *poi = (RooRealVar*)mc->GetParametersOfInterest()->first();
    double mu_hat = poi->getVal();
    double mu_hat_err_up = poi->getErrorHi();
    double mu_hat_err_down = poi->getErrorLo();

    cout << setw(25);
    cout << "POI =";
    cout << Form(" %+.3f",mu_hat);
    cout << Form(" %+.3f",mu_hat_err_up);
    cout << " /";
    cout << Form(" %+.3f",mu_hat_err_down);
    cout << endl;
```

And now for all the NPs;
```C++
    for(auto np_tmp : *mc->GetNuisanceParameters()){
        RooRealVar* np = (RooRealVar*)np_tmp;
        
        string np_name = np->GetName();
        
        double np_value = np->getVal();
        double np_err_up = np->getErrorHi();
        double np_err_down = np->getErrorLo();
        
        cout << setw(25);
        cout << np_name << " =";
        cout << Form(" %+.3f",np_value);
        cout << Form(" %+.3f",np_err_up);
        cout << " /";
        cout << Form(" %+.3f",np_err_down);
        cout << endl;
    }
```

**Exercise 1**:

Let's try to produce the results of a background-only fit, and compare the output with that of the nominal S+B fit. Hint: to perform a B-only fit, the easiest solution is to fix the POI (to what value?)

**Exercise 2**:

Let's now try to make a fit using Minos (hint: look at one of the previous part examples).

**Exercise 3**:

Finally, try to fit the "Asimov" dateset, i.e. pseudo-data set to be the same as the prediction in each bin (hint: in the workspace, there's another dataset saved...)



---

### Systematic Impact <a name="syst"></a>

**Method 1**: "nuisance parameter ranking"

This part shows an example of a way to quantify the impact of each of the individual sources of systematic uncertainties, using the so-called "ranking plot" method. Each of the nuisance parameters is shifted up and down by its uncertainty, and then the fit is repeated. Then the impact of associated systematic is quoted as the resulting shift of the result of the fit (in terms of fitted value of the parameter of interest).

We start from the following macro:
```C++
using namespace RooFit;
using namespace RooStats;
using namespace HistFactory;

void Ranking(){
    TFile *f = new TFile("ws/ws_minimal_combined_meas_model.root");
    RooWorkspace *w = (RooWorkspace*)f->Get("combined");
    RooAbsData *dataset = w->data("obsData");
}
```

Then we get the ModelConfig and we extract from it the POI:
```C++
    ModelConfig *mc = (ModelConfig*)w->obj("ModelConfig");
    RooRealVar *poi = (RooRealVar*)mc->GetParametersOfInterest()->first();
```

We then perform the nominal S+B fit to data (but forcing RooFit and Minuit to stay silent first), 
and we extract and save the fitted value of our POI, as well as its uncertainty (using Minos and trying to kill all the automatic terminal output from RooFit and Minuit):
```C++
    RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
    RooAbsReal *nll = w->pdf("simPdf")->createNLL(*dataset,RooFit::PrintLevel(-1));
    RooMinimizer m(*nll);
    m.setPrintLevel(-1);
    m.migrad();
    m.hesse();
    m.minos();
    double mu_hat = poi->getVal();
    double mu_hat_err_up = poi->getErrorHi();
    double mu_hat_err_down = poi->getErrorLo();
```

Now don't forget to save a snaphot (e.g. named "nominal_snapshot").
```C++
    w->saveSnapshot("nominal_snapshot", *mc->GetPdf()->getParameters(dataset));
```

Then print the nominal fit result:
```C++
    cout << "--------------------------------------------------------" << endl;
    cout << "Nominal fit:" << endl;
    cout << setw(13);
    cout << poi->GetName() << " = ";
    cout << Form("%+.3f ",mu_hat);
    cout << Form("%+.3f",mu_hat_err_up);
    cout << " / ";
    cout << Form("%+.3f",mu_hat_err_down);
    cout << endl;
```

Now the main part, to be done as **exercise**:
  * loop on all the nuisance parmeters (NPs) in the model (excluding the free-floating parameters, hence restricting to those named `alpha_*`
  * save value and error (up/down) of each NP from the nominal fit
  * for each NP, repeat the fit after fixing the NP to nominal value +/- its post-fit uncertainty (to get the "post-fit impact of this NP")
  * for each NP, repeat the fit after fixing the NP to nominal value +/- 1 (to get the "pre-fit impact of this NP")
  * to get the "impact", compare fitted values of POI with nominal fit result in each case
  * take care of loading the snapshot we created before, prior to any new fit
  * print everything in an ordered way

Hints:
  * before the loop, in order to have a nice table printed, add these lines:
```C++
    cout << "--------------------------------------------------------" << endl;
    cout << "NP Ranking:" << endl;
    cout << "             NP name   Pre-fit impact    Post-fit impact" << endl; 
```
  * to loop over the NPs, do something like this:
```C++
for(auto np_tmp : *mc->GetNuisanceParameters()){
    RooRealVar* np = (RooRealVar*)np_tmp;
    string np_name = np->GetName();
```
at this point, we need to check the name, and if it looks like `"alpha_*"` we should skip the parameter and move to the next one, so something like this:
```C++
    if(np_name.find("alpha_")==string::npos) continue;
```
  * **important:** better to load the saved snapshot inside the "for" loop!
  * then we should save the fitted value (`getVal`) and the errors (`getErrorHi` and `getErrorLo`) of the parameter `np` into three `double` valriables, calling them `np_value`, `np_err_up` and `np_err_down`
  * load the snapshot again (`"nominal_snapshot"`), set the value of `np` to `np_value + np_err_up`, set it to constant (`setConstant(true)`) and make a new fit (no need to use Minos here, so `w->pdf("simPdf")->fitTo(...`, with `RooFit::PrintLevel(-1)` as one of the options)
  * after the fit is done, save the fitted value of mu: `double mu_hat_postfit_up = poi->getVal();`
  * then repeat everything for post-fit down (so setting `np` to `np_value + np_err_down`), for prefit up (`np` set to `np_value + 1`) and down (`np` set to `np_value - 1`)
  * don't forget to load the snaphot **before every fit**
  * finally, to pring everything, do something like this:
```C++
        cout << setw(20);
        cout << np_name << "   ";
        cout << Form("%+.3f",mu_hat_prefit_up - mu_hat);
        cout << " / ";
        cout << Form("%+.3f",mu_hat_prefit_down - mu_hat);
        cout << "   ";
        cout << Form("%+.3f",mu_hat_postfit_up - mu_hat);
        cout << " / ";
        cout << Form("%+.3f",mu_hat_postfit_down - mu_hat);
        cout << endl;
```
  * as a final cosmetic touch, after the loop is over, close the printed table:
```C++
    cout << "--------------------------------------------------------" << endl;
    cout << endl;
```


**Method 2**: "grouped nuisance parameter impact"

This part shows an example of a way to quantify the impact of systematic uncertainties, by splitting the statisical component and a number of systematic categories. The method used is the so-called Grouped Impact. It consists in fixing a number of nuisance parameters to their post-fit values and perform fits with each group fixed, and then extracting the impact of each group by comparing the resulting total uncertainty on the parameter of interest with the one from the nominal fit.

We start from the following macro:
```C++
void GroupedImpact(){
    TFile *f = new TFile("ws/ws_minimal_combined_meas_model.root");
    RooWorkspace *w = (RooWorkspace*)f->Get("combined");
    RooAbsData *dataset = w->data("obsData");

    ModelConfig *mc = (ModelConfig*)w->obj("ModelConfig");
    RooRealVar *poi = (RooRealVar*)mc->GetParametersOfInterest()->first();

    RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
    w->pdf("simPdf")->fitTo(*dataset,RooFit::PrintLevel(-1));
    w->saveSnapshot("nominal_snapshot", *mc->GetPdf()->getParameters(dataset));

    double mu_hat = poi->getVal();
    double mu_hat_err_up = poi->getErrorHi();
    double mu_hat_err_down = poi->getErrorLo();
    
    cout << "--------------------------------------" << endl;
    cout << "Nominal fit:" << endl;
    cout << setw(13);
    cout << poi->GetName() << " = ";
    cout << Form("%+.3f ",mu_hat);
    cout << Form("%+.3f",mu_hat_err_up);
    cout << " / ";
    cout << Form("%+.3f",mu_hat_err_down);
    cout << endl;
}
```

The code for the moment is just loading the workspace and performing the nominal fit, printing the results.

Now, let's compute the statistical uncertainty. To do it, we fix all the nuisance parameters:
```C++
    for(auto np_tmp : *mc->GetNuisanceParameters()){
        RooRealVar* np = (RooRealVar*)np_tmp;
        np->setConstant(true);
    }
```
and then we **redo the fit**. 
Notice that the NPs got fixed to their post-fit values, i.e. their current values. 
After this new fit, we need to store the error (which will be the stat-only error):
```C++
    double statUnc_up = poi->getErrorHi();
```
(as **exercise**, add a line for `statUnc_down`), 
and we can also compute the systematic error:
```C++
    double systUnc_up = sqrt(mu_hat_err_up*mu_hat_err_up - statUnc_up*statUnc_up);
```
(again, add a line for `systUnc_down`).
We can then start to print our table, and fill it with what we have so far:
```C++
    cout << "--------------------------------------" << endl;
    cout << "Grouped Impact Table:" << endl;
    
    cout << setw(23);
    cout << "Stat. uncertainty = ";
    cout << Form("%+.3f",statUnc_up);
    cout << " / ";
    cout << Form("%+.3f",statUnc_down);
    cout << endl;

    cout << setw(23);
    cout << "Total systematics = ";
    cout << Form("%+.3f",systUnc_up);
    cout << " / ";
    cout << Form("%+.3f",systUnc_down);
    cout << endl;
```

Finally, let's loop again over the NPs, and add a line to the table for the impact of each of the systematics:
```C++
    for(auto np_tmp : *mc->GetNuisanceParameters()){
        w->loadSnapshot("nominal_snapshot");
        RooRealVar* np = (RooRealVar*)np_tmp;
        string np_name = np->GetName();
        if(np_name.find("alpha_")==string::npos) continue;
        
        np->setConstant(true);
```
then **do a fit**, save the errors up and down to variables called `mu_hat_err_up_tmp` and `mu_hat_err_down_tmp`,
and finally get `npUnc_up` and `npUnc_down` as the difference in quadrature between the total errors (up and down) and these reduced errors. 

Then print them in a nice way and close the loop:
```C++
        cout << setw(20);
        cout << np_name << " = ";
        cout << Form("%+.3f",npUnc_up);
        cout << " / ";
        cout << Form("%+.3f",npUnc_down);
        cout << endl;
    }
    cout << "--------------------------------------" << endl;
    cout << endl;
```

**Exercise 1**: obtain the same table, but using Minos for all the fits.

**Exercise 2**: obtain the same table, using Asimov data and compare.


---

### Discovery significance <a name="setup"></a>

The last exercise is about getting a discovery significance, based on the p0-value calculation. 

The p-value calculation is very simple (see https://arxiv.org/pdf/1007.1727.pdf).
It's given by: $q_0 = 2($NLL$_0 - $NLL$)$, where NLL$_0$ is the negative log-likelihood calculated when the POI is set to zero (background-only hypothesis), and NLL is the value when also the POI is free to float.

Let's load the workspace information as usual:
```C++
using namespace RooFit;
using namespace RooStats;
using namespace HistFactory;

void Significance(){
    TFile *f = new TFile("ws/ws_minimal_combined_meas_model.root");
    RooWorkspace *w = (RooWorkspace*)f->Get("combined");
    RooAbsData *dataset = w->data("obsData");

    ModelConfig *mc = (ModelConfig*)w->obj("ModelConfig");
    RooRealVar *poi = (RooRealVar*)mc->GetParametersOfInterest()->first();
    RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
    w->saveSnapshot("prefit_snapshot", *mc->GetPdf()->getParameters(dataset));
}
```

Now we need to:
  * perform two fits, a normal one and one where we fix the POI to 0 (B-only fit, or conditional fit);
  * each time we save the results in a `RooFitResult` object;
  * from each of the two `RooFitResult` objects we extract the minimum NLL, e.g.:
```C++
    double nll = r->minNll();
```
  * we then extract the p-value from the difference between the two NLL values:
```C++
    float p0 = TMath::Prob(2*(nll0 - nll),1);
    cout << "p-value      = " << p0 << endl;
```
  * we finally turn `p0` to a number of Gaussian standard deviations (which we call "Significance"):
```C++
    float Z = RooStats::PValueToSignificance(p0);
    cout << "Significance = " << Z << endl;
```
