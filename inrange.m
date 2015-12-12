function [OutputArray] = inrange(InputArray,min,max)
    OutputArray = (InputArray>=min).*(InputArray<=max);
end