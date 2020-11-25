#See https://aka.ms/containerfastmode to understand how Visual Studio uses this Dockerfile to build your images for faster debugging.

FROM mcr.microsoft.com/dotnet/runtime:5.0-buster-slim AS base
WORKDIR /app

FROM mcr.microsoft.com/dotnet/sdk:5.0-buster-slim AS build
WORKDIR /src
COPY ["SmartLock.Worker/SmartLock.Worker.csproj", "SmartLock.Worker/"]
RUN dotnet restore "SmartLock.Worker/SmartLock.Worker.csproj"
COPY . .
WORKDIR "/src/SmartLock.Worker"
RUN dotnet build "SmartLock.Worker.csproj" -c Release -o /app/build

FROM build AS publish
RUN dotnet publish "SmartLock.Worker.csproj" -c Release -o /app/publish

FROM base AS final
WORKDIR /app
COPY --from=publish /app/publish .
ENTRYPOINT ["dotnet", "SmartLock.Worker.dll"]