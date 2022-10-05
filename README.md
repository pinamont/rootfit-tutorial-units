# Tutorial "Introduction to RooFit"

University of Trieste, October 2022

Dr. Michele Pinamonti 


---

## Setup

Using git:
  * connect to lxplus or INFN ts farm with ssh via terminal
  * clone the git project:
```
git clone ... RooFitTutorial
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

## Getting started


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

### Performing the fit step by step

With RooFit, we performed the fit with the simple method `fitTo()`. 

Alternatively, one can call the individual steps of the fit procedure individually:

  * create a negative-log-likelihood object, from the model applied to the data (remind: `RooAbsReal` is a generic "function" in the RooFit language):
```C++
RooAbsReal *nll = model.createNLL(data);
```
  * call the minimization process ("Migrad"):
```C++
nll->migrad();
```
  * call a proper method to get the (approximate) uncertainties on the estimated parameters using estimated second derivatives at the NLL minimum ("Hesse"): 
```C++
nll->hesse();
```
  * eventually call another function to get more accurate estimates of the uncertainties by scanning the Likelihood function, eventually catchning asymmetric errors ("Minos"):
```C++
nll->minos();
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


---

## Workspace and factory


### Hello World with Workspace and factory language

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


---

## Unbinned data and likelihood fits

### Hello World unbinned

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
